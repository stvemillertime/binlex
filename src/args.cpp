#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "args.h"

using namespace binlex;

Args::Args(){
    SetDefault();
}

void Args::SetDefault(){
    options.input = NULL;
    options.threads = 1;
    options.help = false;
    options.output = NULL;
    options.list_modes = false;
    options.mode = NULL;
    options.io_type = ARGS_IO_TYPE_UNKNOWN;
    options.pretty = false;
}

bool Args::check_mode(char *mode){
    for (int i = 0; i < ARGS_MODE_COUNT; i++){
        if (strcmp(modes[i], mode) == 0){
            return true;
        }
    }
    return false;
}

int Args::is_file(const char *path){
    struct stat path_stat;
    if (stat(path, &path_stat) != 0){
        return 0;
    }
    return S_ISREG(path_stat.st_mode);
}

int Args::is_dir(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0){
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

void Args::set_io_type(char *input){
    if (is_file(input) != 0){
        options.io_type = ARGS_IO_TYPE_FILE;
    } else if (is_dir(input) != 0){
        options.io_type = ARGS_IO_TYPE_DIR;
    } else{
        options.io_type = ARGS_IO_TYPE_UNKNOWN;
        fprintf(stderr, "unknown input type\n");
        exit(1);
    }
}

void Args::print_help(){
    printf(
        "binlex %s - A Binary Genetic Traits Lexer\n"
        "  -i  --input\t\tinput file\t\t(required)\n"
        "  -m  --mode\t\tset mode\t\t(required)\n"
        "  -lm --list-modes\tlist modes\n"
        "  -h  --help\t\tdisplay help\n"
        "  -o  --output\t\toutput file\t\t(optional)\n"
        "  -p  --pretty\t\tpretty output\t\t(optional)\n"
        "  -v  --version\t\tdisplay version\n"
        "Author: @c3rb3ru5d3d53c\n",
        version
    );
}

void Args::parse(int argc, char **argv){
    if (argc < 2){
        print_help();
        exit(0);
    }
    for (int i = 0; i < argc; i++){
        if (strcmp(argv[i], (char *)"-h") == 0 ||
            strcmp(argv[i], (char *)"--help") == 0){
            options.help = true;
            print_help();
            exit(0);
        }
        if (strcmp(argv[i], (char *)"-v") == 0 ||
            strcmp(argv[i], (char *)"--version") == 0){
            options.help = true;
            printf("%s\n", version);
            exit(0);
        }
        if (strcmp(argv[i], (char *)"-lm") == 0 ||
            strcmp(argv[i], (char *)"--list-modes") == 0){
            options.list_modes = true;
            for (int j = 0; j < ARGS_MODE_COUNT; j++){
                printf("%s\n", modes[j]);
            }
            exit(0);
        }
        if (strcmp(argv[i], (char *)"-i") == 0 ||
            strcmp(argv[i], (char *)"--input") == 0){
            options.input = argv[i+1];
            set_io_type(options.input);
        }
        if (strcmp(argv[i], (char *)"-p") == 0 ||
            strcmp(argv[i], (char *)"--pretty") == 0){
            options.pretty = true;
        }
        if (strcmp(argv[i], (char *)"-o") == 0 ||
            strcmp(argv[i], (char *)"--output") == 0){
            options.output = argv[i+1];
        }
        if (strcmp(argv[i], (char *)"-m") == 0 ||
            strcmp(argv[i], (char *)"--mode") == 0){
            options.mode = argv[i+1];
            if (check_mode(options.mode) == false){
                fprintf(stderr, "%s is an invalid mode\n", options.mode);
                exit(1);
            }
        }
    }
}

Args::~Args(){
    SetDefault();
}
