import os
from zipfile import ZipFile
import subprocess
from glob import glob

def build_delay_checker():
    delay_checker_solution = 'D:\Projects\DelayChecker\DelayChecker.sln'

    ms_build_dir = os.path.expandvars(os.path.expandvars('%TimexCommonMSBuildPath%'))
    print(ms_build_dir)
    if os.path.exists(ms_build_dir):
        ms_build = os.path.join(ms_build_dir, 'MSBuild.exe')
    else:
        ms_build = r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe'

    arg_action = '/t:Rebuild'
    arg_conf = '/p:Configuration=Release'
    arg_platform = '/p:Platform=x64'
    ms_build = os.path.join(ms_build_dir, 'MSBuild.exe')
    p = subprocess.call(
        [ms_build, delay_checker_solution, arg_action, arg_conf, arg_platform])
    if p != 0:
        raise SystemExit(p)

def pack_delay_checker():
    delay_checker_package_path = '.\DelayChecker.zip'
    delay_checker_build_paths = glob(r'D:\Projects\DelayChecker\x64\Release\*')
    run_script = r'D:\Projects\DelayChecker\scripts\DelayChecker.cmd'
    with ZipFile(delay_checker_package_path, 'w') as zf:
        for build_file in delay_checker_build_paths:
            print(build_file)
            zf.write(build_file, arcname=os.path.basename(build_file))
        zf.write(run_script, arcname=os.path.basename(run_script))

if __name__ == '__main__':
    build_delay_checker()
    pack_delay_checker()
