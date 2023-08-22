use std::net::SocketAddr;

use basil_tonic::socket;
use clap::Parser;

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Options {
    /// Turn debugging information on
    #[arg(short, long, action = clap::ArgAction::Count)]
    verbose: u8,

    #[clap(flatten)]
    listen: Listen,
}

#[derive(Debug, clap::Args)]
#[group(required = true, multiple = false)]
pub struct Listen {
    /// Listening port
    #[clap(long, value_name = "PORT")]
    port: Option<u16>,
    /// Socket name create by the service manager which needs to be activated
    #[arg(short = 's', long, value_name = "NAME")]
    pub activate_socket: Option<String>,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let options = Options::parse();
    setup_tracing(options.verbose);

    let result = run(options);
    if let Err(error) = &result {
        tracing::error!(%error, "server error");
    }

    Ok(())
}

#[tokio::main]
async fn run(options: Options) -> Result<(), Box<dyn std::error::Error>> {
    let router = tonic::transport::Server::builder().add_service(basil_tonic::make_service());

    let listener = if let Some(name) = &options.listen.activate_socket {
        let mut socket = socket::activate_socket(name)?.take::<std::net::TcpListener>();
        let socket = if socket.len() == 1 {
            socket.swap_remove(0)
        } else {
            return Err(std::io::Error::new(
                std::io::ErrorKind::Other,
                "require exactly one named socket",
            )
            .into());
        };
        socket.set_nonblocking(true)?;
        tokio::net::TcpListener::from_std(socket)?
    } else if let Some(port) = options.listen.port {
        let addr = SocketAddr::from(([127u8, 0, 0, 1], port));
        tokio::net::TcpListener::bind(addr).await?
    } else {
        // not reachable due to clap  group: required = true, multiple = false
        unreachable!("either socket name or port must be provided");
    };

    let addr = listener.local_addr()?;
    tracing::info!(%addr, "listening");

    let listener = tokio_stream::wrappers::TcpListenerStream::new(listener);
    router.serve_with_incoming(listener).await?;

    Ok(())
}

fn setup_tracing(level: u8) {
    use tracing_subscriber::filter::EnvFilter;

    let env_name = format!("{}_LOG", env!("CARGO_PKG_NAME").to_uppercase());

    let filter = if level > 0 {
        let log_level = "debug";
        EnvFilter::default()
            .add_directive("info".parse().expect("directive"))
            .add_directive(
                format!("basil_tonic={log_level}")
                    .parse()
                    .expect("directive"),
            )
            .add_directive(
                format!("bazel_proto={log_level}")
                    .parse()
                    .expect("directive"),
            )
            .add_directive(
                format!("compile_commands={log_level}")
                    .parse()
                    .expect("directive"),
            )
    } else if let Ok(filter) = EnvFilter::try_from_env(env_name) {
        dbg!(&filter);
        filter
    } else {
        EnvFilter::default().add_directive("info".parse().expect("directive"))
    };

    tracing_subscriber::fmt().with_env_filter(filter).init();
}
