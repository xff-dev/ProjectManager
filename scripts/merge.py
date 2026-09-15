# used to merge all source files into a single file for easier code review by an LLM 

import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.dirname(SCRIPT_DIR)
OUTPUT_FILE = os.path.join(REPO_ROOT, "all_code.txt")
EXTENSIONS = (".cpp", ".h", ".hpp", ".cc", ".cxx")

def collect_cpp_files(name):
    files = []
    for dirpath, _, filenames in os.walk(os.path.join(REPO_ROOT, name)):
        for f in filenames:
            if f.endswith(EXTENSIONS):
                files.append(os.path.join(dirpath, f))
    return sorted(files)

def merge_files(files, output):
    with open(output, "w", encoding="utf-8") as out:
        for path in files:
            rel = os.path.relpath(path, REPO_ROOT)
            out.write(f"\n\n// ===== {rel} =====\n\n")
            with open(path, "r", encoding="utf-8", errors="ignore") as f:
                out.write(f.read().strip() + "\n")
    print(f"✅ Combined {len(files)} files into {output}")

if __name__ == "__main__":
    files = collect_cpp_files("src")
    files += collect_cpp_files("tests")
    files.append(os.path.join(REPO_ROOT, "CMakeLists.txt"))
    files.append(os.path.join(REPO_ROOT, "project.ini"))

    merge_files(files, OUTPUT_FILE)