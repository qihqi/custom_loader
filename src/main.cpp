#include "loader.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>

using namespace loader;

struct PythonAPI {
  PythonAPI() {
    auto global = SystemLibrary::create();
    find_shared_function_ = CustomLibrary::create("libfind_shared_function.so");
    find_shared_function_->add_search_library(global);
    find_shared_function_->load();

    python_ = CustomLibrary::create(PYTHON_SO_PATH);
    python_->add_search_library(find_shared_function_);
    python_->add_search_library(global);
    python_->load();

    auto find_shared_python_ref = (CustomLibraryPtr*) find_shared_function_->sym("the_python_library").value();
    *find_shared_python_ref = python_;

    python_runner_ = CustomLibrary::create("libpython_runner.so");
    python_runner_->add_search_library(python_);
    python_runner_->add_search_library(global);
    python_runner_->load();
  }
  void run(const char* code) {
    auto run = (void(*)(const char* code)) python_runner_->sym("run").value();
    run(code);
  }
  CustomLibraryPtr find_shared_function_;
  CustomLibraryPtr python_;
  CustomLibraryPtr python_runner_;
};

auto example_src = R"end(
print("I think None is", id(None))
from time import time

def fib(x):
  if x <= 1:
    return 1
  return fib(x - 1) + fib(x - 2)

def do_fib():
  s = time()
  fib(30)
  e = time()
  print(e - s)

)end";

auto run_numpy = R"end(
import numpy as np
print(np.arange(10)*10)
)end";

void runX(const char* name, const char* statement) {
  PythonAPI a;
  std::cout << "=============== "<< name << " ===================" << std::endl;
  a.run(statement);
}


int main(int argc, const char **argv) {
  if (argc > 1)  {
    std::ifstream f(argv[1]);
    std::stringstream buffer;
    buffer << f.rdbuf();
    runX(argv[1], buffer.str().c_str());
  } else {
    std::cerr << "No file name provided..." << std::endl;
  }
}

