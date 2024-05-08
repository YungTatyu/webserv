#! /usr/bin/python3

import os
import sys
import subprocess
from colorama import Fore, Style


def printLog(passed, failed):
    print("[============== test result ==============]")
    print(f"[========]    {passed + failed} tests ran")
    print(f"{Fore.GREEN}[ PASSED ]{Style.RESET_ALL}    {passed} tests")
    print(f"{Fore.RED}[ FAILED ]{Style.RESET_ALL}    {failed} tests")


def run_test(command):
    result = subprocess.Popen(
        command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT
    )
    exit_status = result.wait()

    output_bytes, _ = result.communicate()  # 標準エラー出力を取得
    output = output_bytes.decode("utf-8")  # バイト列を文字列にデコード

    if exit_status != 0:
        print(f'output: "{output}"', file=sys.stderr)
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

    ti = 0
    passed_cnt = 0
    failed_cnt = 0
    for test in test_cases:
        print(f"[ test{ti} ] {test}", end="\n")
        result = run_test(test)
        if result == 0:
            print(f"{Fore.GREEN}[ PASS ]{Style.RESET_ALL}", end="\n\n", flush=True)
            passed_cnt += 1
        else:
            print(
                f"{Fore.RED}[ FAIL ]{Style.RESET_ALL}",
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
