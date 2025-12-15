#define _POSIX_C_SOURCE 200809L // strtok_r için gerekli
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
 
#define MAX_INPUT 1024
#define MAX_ARGS 100
 
// Projenin istediği standart hata mesajı
void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}
 
// Trim trailing newline and spaces
void trim(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[len - 1] = '\0';
        len--;
    }
}
 
// Önceki path girdilerini temizlemek için
void free_paths(char *paths[]) {
    for (int i = 0; paths[i] != NULL; i++) {
        free(paths[i]); 
        paths[i] = NULL;
    }
}
 
// Çocuğun komutu çalıştırması için ana fonksiyon
void run_command(char *args[], char *outfile, char *path_list[]) {
    if (outfile != NULL) {
        close(STDOUT_FILENO); 
        int fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (fd < 0) { 
            print_error();
            exit(1); 
        }
    }
 
    char full_path[MAX_INPUT];
    if (strchr(args[0], '/') != NULL) {
        if (access(args[0], X_OK) == 0) {
            execv(args[0], args);
        }
    } else {
        for (int j = 0; path_list[j] != NULL; j++) {
            snprintf(full_path, sizeof(full_path), "%s/%s", path_list[j], args[0]);
            if (access(full_path, X_OK) == 0) {
                execv(full_path, args);
            }
        }
    }
    print_error(); 
    exit(1);
}
 
// Komut satırını işleyen ana fonksiyon
void process_line(char *line, char *path_list[]) {
    trim(line);
    if (strlen(line) == 0)
        return; 
    pid_t pids[MAX_ARGS];
    int pid_count = 0;
 
    char *save_cmd; 
    char *cmd_str;
    int cmd_count = 0;
    // char* all_cmds[MAX_ARGS]; // <-- HATALI SATIR KALDIRILDI
 
    char *input_copy = strdup(line); 
    for (cmd_str = strtok_r(input_copy, "&", &save_cmd); cmd_str != NULL; cmd_str = strtok_r(NULL, "&", &save_cmd)) {
        // all_cmds[cmd_count++] = cmd_str; // <-- HATALI SATIR DEĞİŞTİRİLDİ
        cmd_count++; // <-- DÜZELTİLDİ
    }
    free(input_copy);
 
    cmd_str = strtok_r(line, "&", &save_cmd);
    while (cmd_str != NULL) {
        char *args[MAX_ARGS];
        int i = 0;
        char *outfile = NULL;
 
        // --- YÖNLENDİRME PARSING MANTIĞI ---
        char *redirect_ptr = strchr(cmd_str, '>');
        if (redirect_ptr != NULL) {
            *redirect_ptr = '\0'; 
            char *outfile_str = redirect_ptr + 1;
 
            while (*outfile_str == ' ' || *outfile_str == '\t') outfile_str++;
            trim(outfile_str);
            if (strlen(outfile_str) == 0 || strchr(outfile_str, ' ') != NULL || strchr(outfile_str, '\t') != NULL || strchr(redirect_ptr + 1, '>') != NULL) {
                print_error();
                pid_count = 0;
                break; 
            }
            outfile = outfile_str;
        }
 
        // Komutu boşluklara göre ayır
        char *save_arg; 
        char *token = strtok_r(cmd_str, " \t", &save_arg);
        while (token != NULL) {
            args[i++] = token;
            token = strtok_r(NULL, " \t", &save_arg);
        }
        args[i] = NULL;
 
        if (args[0] == NULL) {
            if (outfile != NULL) {
                print_error();
                pid_count = 0;
                break;
            }
            cmd_str = strtok_r(NULL, "&", &save_cmd);
            continue; 
        }
        // --- PARSING SONU ---
 
 
        // GÖMÜLÜ KOMUTLAR
        if (strcmp(args[0], "exit") == 0) {
            if (cmd_count > 1 || args[1] != NULL || outfile != NULL) { 
                print_error();
                pid_count = 0; 
                break; 
            } else {
                free_paths(path_list);
                exit(0); 
            }
        } 
        else if (strcmp(args[0], "cd") == 0) {
            if (cmd_count > 1 || args[1] == NULL || args[2] != NULL || outfile != NULL) { 
                print_error();
                pid_count = 0; 
                break; 
            } 
            else if (chdir(args[1]) != 0) {
                print_error();
                pid_count = 0;
                break; 
            }
            cmd_str = strtok_r(NULL, "&", &save_cmd);
            continue; 
        }
        else if (strcmp(args[0], "path") == 0) {
            if (cmd_count > 1 || outfile != NULL) { 
                print_error();
                pid_count = 0;
                break;
            } else {
                free_paths(path_list);
                int j = 0;
                while (args[j + 1] != NULL && j < MAX_ARGS - 1) {
                    path_list[j] = strdup(args[j + 1]);
                    j++;
                }
                path_list[j] = NULL;
            }
            cmd_str = strtok_r(NULL, "&", &save_cmd);
            continue;
        }
 
        // DIŞ KOMUTLAR
        pid_t pid = fork();
        if (pid == 0) {
            run_command(args, outfile, path_list);
        } else if (pid > 0) {
            pids[pid_count++] = pid; 
        } else {
            print_error();
        }
        cmd_str = strtok_r(NULL, "&", &save_cmd);
    }
 
    for (int k = 0; k < pid_count; k++) {
        waitpid(pids[k], NULL, 0);
    }
}
 
 
int main(int argc, char *argv[]) {
    char *path_list[MAX_ARGS];
    path_list[0] = strdup("/bin"); 
    path_list[1] = NULL;
 
    FILE *input_stream;
 
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
 
    if (argc == 1) {
        input_stream = stdin;
    } else if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            print_error();
            exit(1); 
        }
    } else {
        print_error();
        exit(1);
    }
    if (argc == 1) {
        write(STDOUT_FILENO, "wish> ", 7);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, input_stream)) != -1) {
        process_line(line, path_list); // Okunan satırı işle
        if (argc == 1) {
            write(STDOUT_FILENO, "wish> ", 7);
        }
    }
 
    free(line);
    if (argc == 2) {
        fclose(input_stream); 
    }
    free_paths(path_list);
    return 0;
}