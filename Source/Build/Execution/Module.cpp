﻿module;

#include <any>
#include <ctime>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

export module Soup.Build.Execution;

import json11;
import Opal;
import Soup.Build;
import Soup.Build.Utilities;

using namespace Opal;

#include "BuildHistory.h"
#include "BuildHistoryChecker.h"
#include "BuildHistoryJson.h"
#include "BuildHistoryManager.h"
#include "BuildRunner.h"