extern "C" {
    // https://developer.apple.com/documentation/xpc/1505523-launch_activate_socket
    #[cfg(target_os = "macos")]
    fn launch_activate_socket(
        name: *const libc::c_char,
        fds: *mut *mut libc::c_int,
        cnt: *mut libc::size_t,
    ) -> libc::c_int;
}

#[cfg(target_family = "unix")]
type RawSocket = std::os::unix::io::RawFd;
#[cfg(target_family = "windows")]
type RawSocket = std::os::windows::io::RawSocket;

#[cfg(target_family = "unix")]
use std::os::unix::io::FromRawFd as FromRaw;
#[cfg(target_family = "windows")]
use std::os::windows::io::FromRawSocket as FromRaw;

pub struct ActivateSocket(Vec<RawSocket>);

impl ActivateSocket {
    pub fn take<T>(self) -> Vec<T>
    where
        T: FromRaw,
    {
        self.0
            .iter()
            .map(|fd| {
                #[cfg(unix)]
                unsafe {
                    T::from_raw_fd(*fd)
                }
                #[cfg(windows)]
                unsafe {
                    T::from_raw_socket(*fd)
                }
            })
            .collect()
    }
}

/// Pass the name of a socket listed in a launchd.plist, receive `RawFd`s.
///
/// See `man launch` for usage of `launch_activate_socket`.
#[cfg(target_os = "macos")]
pub fn activate_socket(name: &str) -> std::io::Result<ActivateSocket> {
    let name = std::ffi::CString::new(name)
        .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidInput, e))?;
    let mut fds: *mut libc::c_int = std::ptr::null_mut();
    let mut cnt: libc::size_t = 0;

    let error = unsafe { launch_activate_socket(name.as_ptr(), &mut fds, &mut cnt) };
    if error != 0 {
        return Err(std::io::Error::from_raw_os_error(error));
    }

    if cnt == 0 {
        return Err(std::io::Error::new(
            std::io::ErrorKind::Other,
            "no socket found",
        ));
    }

    let out = unsafe {
        let out = std::slice::from_raw_parts(fds, cnt).to_vec();
        libc::free(fds as *mut _);
        out
    };

    Ok(ActivateSocket(out))
}

#[cfg(target_family = "windows")]
pub fn activate_socket(_name: &str) -> std::io::Result<ActivateSocket> {
    Err(std::io::Error::new(
        std::io::ErrorKind::Other,
        "not implemented",
    ))
}

#[cfg(all(target_family = "unix", not(target_os = "macos")))]
pub fn activate_socket(name: &str) -> std::io::Result<ActivateSocket> {
    let pid = std::env::var("LISTEN_PID")
        .ok()
        .and_then(|v| v.parse().ok());
    let fds = std::env::var("LISTEN_FDS")
        .ok()
        .and_then(|v| v.parse().ok());
    let fdnames = std::env::var("LISTEN_FDNAMES")
        .ok()
        .and_then(|v| v.parse().ok());
    listenfds(name, pid, fds, fdnames).map(ActivateSocket)
}

#[cfg(all(target_family = "unix", not(target_os = "macos")))]
fn listenfds(
    name: &str,
    pid: Option<u32>,
    fds: Option<std::os::unix::io::RawFd>,
    names: Option<String>,
) -> std::io::Result<Vec<std::os::unix::io::RawFd>> {
    const SD_LISTEN_FDS_START: std::os::unix::io::RawFd = 3;

    let fds =
        fds.ok_or_else(|| std::io::Error::new(std::io::ErrorKind::Other, "LISTEN_FDS missing"))?;
    let names = names
        .ok_or_else(|| std::io::Error::new(std::io::ErrorKind::Other, "LISTEN_FDNAMES missing"))?;
    if !pid.map(|p| p == std::process::id()).unwrap_or(true) {
        return Err(std::io::Error::new(
            std::io::ErrorKind::Other,
            "protocol error, PID does not match",
        ));
    }
    let names = names.split(':').map(String::from).collect::<Vec<_>>();
    if names.len() != fds as usize {
        return Err(std::io::Error::new(
            std::io::ErrorKind::Other,
            "protocol error, LISTEN_FDNAMES length does not match",
        ));
    }
    let result = (SD_LISTEN_FDS_START..(SD_LISTEN_FDS_START + fds))
        .zip(names)
        .filter_map(|(fd, n)| if n == name { Some(fd) } else { None })
        .collect();
    Ok(result)
}

#[cfg(test)]
mod tests {
    #[cfg(all(target_family = "unix", not(target_os = "macos")))]
    #[test]
    fn test_listenfds() {
        use super::listenfds;
        let pid = std::process::id();
        assert!(listenfds("test", None, Some(1), Some("test".into())).is_ok());
        assert!(listenfds("test", Some(pid), Some(1), Some("test".into())).is_ok());
        // Missmatch in process id is an error
        assert!(listenfds("test", Some(1), Some(1), Some("test".into())).is_err());
        // socket not found by name
        assert!(listenfds("test", None, Some(1), Some("mismatch".into()))
            .unwrap()
            .is_empty());
        // mismatch in length
        assert!(listenfds("test", None, Some(2), Some("test".into())).is_err());
    }
}
