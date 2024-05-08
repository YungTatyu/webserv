#! /usr/bin/python3

import os
import sys
import subprocess
from colorama import Fore, Style

TEST_NAME = 'the entire test'

def printLog(passed, failed):
    print(f"[============== {TEST_NAME} result ==============]")
    print(f"[========]    {passed + failed} tests ran")
    print(f"{Fore.GREEN}[ PASSED ]{Style.RESET_ALL}    {passed} tests")
    print(f"{Fore.RED}[ FAILED ]{Style.RESET_ALL}    {failed} tests")

def init(path):
    process = subprocess.Popen(["make", "-j", "-C", path])
    exit_status = process.wait()

    if exit_status != 0:
        sys.exit(exit_status)

def run_test(command):

    result = subprocess.Popen(command, shell=True)
    # testの出力を見せたくない場合
    # result = subprocess.Popen(
    #     command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    # )
    exit_status = result.wait()
    return exit_status

def main():
    CWD = os.path.dirname(os.path.abspath(__file__))
    test_cases = [
        f"{CWD}/integration_test/server_res_test.sh",
        f"pytest {CWD}/integration_test/server_res_header_test.py",
        f"pytest {CWD}/cgi/meta_vars_test.py",
        f"{CWD}/conf/main_parser_test.sh",
        f"{CWD}/conf/directive_err_test.sh",
        f"{CWD}/integration_test/KeepaliveTimeoutTest.sh",
        f"{CWD}/integration_test/ReceiveTimeoutTest.sh",
        f"{CWD}/integration_test/SendTimeoutTest.sh",
    ]

    init(f'{CWD}/..')
    ti = 0
    passed_cnt = 0
    failed_cnt = 0
    for test in test_cases:
        CUR_TEST = f'{TEST_NAME}{ti}'
        print(f"[   {CUR_TEST}   ] {test}", end="\n")
        result = run_test(test)
        if result == 0:
            print(f"{Fore.GREEN}[   PASS   ]{Style.RESET_ALL} {CUR_TEST}:{test}", end="\n\n", flush=True)
            passed_cnt += 1
        else:
            print(
                f"{Fore.RED}[   FAIL   ]{Style.RESET_ALL} {CUR_TEST}:{test}",
                file=sys.stderr,
                end="\n\n",
                flush=True,
            )
            failed_cnt += 1
        ti += 1

    printLog(passed_cnt, failed_cnt)

    if failed_cnt != 0:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
