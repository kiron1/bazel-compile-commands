#pragma once

// MSVC CRT defines environ as a macro (*__p__environ()), which conflicts with
// the 'environ' field accessor generated for RepositoryRuleInfo in
// stardoc_output.pb.h (included transitively via analysis_v2.pb.h).
#ifdef environ
#undef environ
#endif
#include "src/main/protobuf/analysis_v2.pb.h"
