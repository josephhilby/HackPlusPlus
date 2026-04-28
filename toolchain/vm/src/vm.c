#include "vm.h"

static int is_directory(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int has_vm_ext(const char* name) {
    const char* dot = strrchr(name, '.');
    return dot && strcmp(dot, ".vm") == 0;
}

static int translate_file(FILE* dest, const char* in) {
    FILE* src = fopen(in, "r");
    if (!src) {
        printf("Error opening source file: %s\n", in);
        return 1;
    }

    set_filename(in);

    char buffer[MAX_LENGTH];
    while (fgets(buffer, MAX_LENGTH, src)) {
        Operation* next = lex_line(buffer);
        if (!next) {
            continue;
        }

        parse_operation(next);

        if (next->command[0] == '\0') {
            free(next);
            continue;
        }

        generate(dest, next);
        free(next);
    }

    fclose(src);
    return 0;
}

static int compare_paths(const void* a, const void* b) {
    const char* const* pa = a;
    const char* const* pb = b;

    return strcmp(*pa, *pb);
}

static int translate_dir(FILE* dest, const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        printf("Error opening directory: %s\n", dir_path);
        return 1;
    }

    char* files[256];
    size_t count = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!has_vm_ext(entry->d_name)) {
            continue;
        }

        if (count >= 256) {
            printf("Too many VM files in directory: %s\n", dir_path);
            closedir(dir);
            return 1;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        files[count] = malloc(strlen(path) + 1);
        if (!files[count]) {
            closedir(dir);
            return 1;
        }

        strcpy(files[count], path);
        count++;
    }

    closedir(dir);

    qsort(files, count, sizeof(files[0]), compare_paths);

    int rc = 0;
    for (size_t i = 0; i < count; i++) {
        if (translate_file(dest, files[i]) != 0) {
            rc = 1;
            break;
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(files[i]);
    }

    return rc;
}

int translate_vm(const char* in, const char* out) {
    reset_generator();

    FILE* dest = fopen(out, "w");
    if (!dest) {
        printf("Error opening output file: %s\n", out);
        return 1;
    }

    int rc;
    if (is_directory(in)) {
        generate_bootstrap(dest);
        rc = translate_dir(dest, in);
    } else {
        rc = translate_file(dest, in);
    }

    fclose(dest);
    return rc;
}
