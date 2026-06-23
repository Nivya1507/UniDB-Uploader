import os
import urllib.request
import subprocess
import shutil

PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
BACKEND_DIR = os.path.join(PROJECT_DIR, "backend")
FRONTEND_DIR = os.path.join(PROJECT_DIR, "frontend")
DATA_DIR = os.path.join(PROJECT_DIR, "data")

DEVKIT_URL = "https://github.com/skeeto/w64devkit/releases/download/v2.8.0/w64devkit-x64-2.8.0.7z.exe"
HTTPLIB_URL = "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"

def make_dirs():
    print("Creating project directories...")
    os.makedirs(BACKEND_DIR, exist_ok=True)
    os.makedirs(FRONTEND_DIR, exist_ok=True)
    os.makedirs(DATA_DIR, exist_ok=True)
    print("Directories created.")

def download_file(url, dest_path):
    print(f"Downloading {url} to {dest_path}...")
    
    # Define progress callback
    def progress(count, block_size, total_size):
        percent = int(count * block_size * 100 / total_size) if total_size > 0 else 0
        if percent % 10 == 0:  # print every 10%
            print(f"Progress: {percent}% done", end="\r")
            
    urllib.request.urlretrieve(url, dest_path, reporthook=progress)
    print("\nDownload complete.")

def extract_devkit(exe_path):
    print("Extracting compiler toolchain...")
    # Run the self-extracting archive in silent mode (-y) and specify the output folder (-o)
    # Extract directly in the project directory
    cmd = [exe_path, "-y", f"-o{PROJECT_DIR}"]
    print(f"Executing: {' '.join(cmd)}")
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode == 0:
        print("Extraction complete.")
    else:
        print("Extraction failed:")
        print(result.stderr.decode("utf-8"))
        raise RuntimeError("Failed to extract compiler")

def main():
    make_dirs()
    
    # 1. Download httplib.h
    httplib_dest = os.path.join(BACKEND_DIR, "httplib.h")
    if not os.path.exists(httplib_dest):
        download_file(HTTPLIB_URL, httplib_dest)
    else:
        print("httplib.h already exists. Skipping.")
        
    # 2. Download and extract w64devkit
    devkit_exe = os.path.join(PROJECT_DIR, "w64devkit.exe")
    devkit_dir = os.path.join(PROJECT_DIR, "w64devkit")
    
    if not os.path.exists(devkit_dir):
        if not os.path.exists(devkit_exe):
            download_file(DEVKIT_URL, devkit_exe)
        try:
            extract_devkit(devkit_exe)
        finally:
            if os.path.exists(devkit_exe):
                print("Cleaning up devkit installer...")
                os.remove(devkit_exe)
    else:
        print("w64devkit toolchain folder already exists. Skipping.")
        
    print("Toolchain setup successfully finished!")

if __name__ == "__main__":
    main()
