#!/bin/bash

# =========================================================
#  Coffee Robot - Setup & Launcher Script
# =========================================================
#
#  This script simplifies the process of installing 
#  dependencies, compiling, and running the project.
#
#  Usage:
#    ./setup.sh           # Install deps, build (release), and run
#    ./setup.sh run       # Same as above
#    ./setup.sh debug     # Build in debug mode and run
#    ./setup.sh build     # Build (release) only, don't run
#    ./setup.sh build debug # Build in debug mode only, don't run
#    ./setup.sh deps      # Install dependencies only
#    ./setup.sh clean     # Clean build files
#    ./setup.sh help      # Show this help message
#
# =========================================================

set -e

# --- Configuration ---
TARGET_EXEC="coffee_robot"
MAKE_FLAGS=""
MODE="run" # Default action

# --- Argument Parsing ---
for arg in "$@"; do
  case "$arg" in
    debug)
      MAKE_FLAGS="DEBUG=1"
      ;;
    build)
      MODE="build"
      ;;
    run)
      MODE="run"
      ;;
    deps)
      MODE="deps"
      ;;
    clean)
      MODE="clean"
      ;;
    -h|--help|help)
      MODE="help"
      ;;
  esac
done

# --- Functions ---
show_help() {
  echo "Usage: $0 [command] [options]"
  grep -E "^#    " "$0" | sed 's/^#    //'
}

header() {
  echo "============================================="
  echo "       ☕ COFFEE ROBOT 3D DEMO ☕"
  echo "============================================="
  echo
}

install_deps() {
  echo "📦 Installing dependencies using 'make install-deps'..."
  make install-deps || {
    echo "⚠️  Failed to install dependencies. Please check your system or install them manually."
    exit 1
  }
  echo "✅ Dependencies are up to date."
  echo
}

build_project() {
  local build_type="Release"
  if [[ -n "$MAKE_FLAGS" ]]; then
    build_type="Debug"
  fi
  echo "🔧 Building project ($build_type mode)..."
  make $MAKE_FLAGS
  echo
}

run_demo() {
  if [ ! -f "$TARGET_EXEC" ]; then
      echo "❌ Executable '$TARGET_EXEC' not found. Please build the project first with './setup.sh build'."
      exit 1
  fi
  echo "🚀 Launching OpenGL Demo..."
  echo "--------------------------------------------"
  ./"$TARGET_EXEC"
  echo "--------------------------------------------"
  echo "👋 Demo finished."
}

clean_project() {
    echo "🧹 Cleaning project..."
    make clean
}


# --- Main Execution Flow ---
header

case "$MODE" in
  help)
    show_help
    ;;
  deps)
    install_deps
    ;;
  build)
    build_project
    echo "✅ Build complete. Run with './setup.sh run' or './$TARGET_EXEC'"
    ;;
  run)
    install_deps
    build_project
    run_demo
    ;;
  clean)
    clean_project
    ;;
esac

exit 0
