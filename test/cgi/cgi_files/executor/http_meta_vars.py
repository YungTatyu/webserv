from all_meta_vars import printEnvVar


def main():
    print("\n", end="")
    env_list = [
        "HTTP_HOST",
        "HTTP_TEST",
        "HTTP_TEST_",
        "HTTP_T_E_S_T",
        "HTTP_TEST_TEST",
        "HTTP_A_BC_DEF",
    ]

    for env in env_list:
        printEnvVar(env)


if __name__ == "__main__":
    main()
