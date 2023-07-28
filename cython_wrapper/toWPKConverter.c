#include <toWPKConverter.h>

#define diff_to_clamp(number, clamp) ((clamp-1) - ((number+clamp-1) & (clamp-1)))
#define clamp_int(number, clamp) (((clamp-1) + number) & ~(clamp-1))


typedef struct {
    uint32_t length;
    uint32_t allocated_length;
    uint32_t* objects;
} uint32_list;

typedef struct audio_data {
    uint32_t id;
    uint32_t length;
    uint8_t* data;
} AudioData;


typedef struct { 
    uint64_t length; 
    AudioData* objects; 
} STATIC_LIST;

static void write_wpk_file(STATIC_LIST* wemFiles, char* outputPath)
{
    printf("\n______________Starting write_wpk_file()_________________");
    const int clamp = 8;
    FILE* wpk_file = fopen(outputPath, "wb");
    if (!wpk_file) {
        printf("Failed to open some wpk output file");
        return;
    }

    // write header
    fwrite("r3d2\1\0\0\0", 8, 1, wpk_file);
    fwrite(&wemFiles->length, 4, 1, wpk_file);

    // skip over the initial offset section and write them later
    fseek(wpk_file, wemFiles->length * 4, SEEK_CUR);
    fseek(wpk_file, diff_to_clamp(ftell(wpk_file), clamp), SEEK_CUR);

    char filename_string[15];
    uint32_list offset_list;
    initialize_list_size(&offset_list, wemFiles->length);
    for (uint32_t i = 0; i < wemFiles->length; i++) {
        // save offset to fill in the offset section later
        add_object(&offset_list, &(uint32_t) {ftell(wpk_file)});
        fseek(wpk_file, 4, SEEK_CUR);
        fwrite(&wemFiles->objects[i].length, 4, 1, wpk_file);
        sprintf(filename_string, "%u.wem", wemFiles->objects[i].id);
        int filename_string_length = strlen(filename_string);
        fwrite(&filename_string_length, 4, 1, wpk_file);
        for (int i = 0; i < filename_string_length; i++) {
            putc(filename_string[i], wpk_file);
            fseek(wpk_file, 1, SEEK_CUR);
        }
        fseek(wpk_file, diff_to_clamp(ftell(wpk_file), clamp), SEEK_CUR);
    }

    uint32_t start_data_offset = ftell(wpk_file);
    for (uint32_t i = 0; i < wemFiles->length; i++) {
        // seek to initial place in the offset section and write offset
        fseek(wpk_file, 12 + 4*i, SEEK_SET);
        fwrite(&offset_list.objects[i], 4, 1, wpk_file);

        // seek to the written offset, update the offset variable for further use and write data offset
        fseek(wpk_file, offset_list.objects[i], SEEK_SET);
        offset_list.objects[i] = i == 0 ? start_data_offset : clamp_int(offset_list.objects[i-1] + wemFiles->objects[i-1].length, clamp);
        fwrite(&offset_list.objects[i], 4, 1, wpk_file);

        // seek to written data offset and write data
        fseek(wpk_file, offset_list.objects[i], SEEK_SET);
        fwrite(wemFiles->objects[i].data, wemFiles->objects[i].length, 1, wpk_file);
    }

    fclose(wpk_file);
    free(offset_list.objects);
    
    printf("\n______________Ending write_wpk_file()_________________");
}

static void printTest() 
{
    printf("test");
}

static void openFile(char* pathToDir, char* wpkName)
{
    FILE *file;
    
    uint8_t dataConfig = 0;
    AudioData wemConfig = {0, 1, &dataConfig};

    STATIC_LIST listTemp = { 0, {&wemConfig} };
    STATIC_LIST *wemList = &listTemp;

    dirent *entry;
    int counter = 0;

    DIR *dir = opendir(pathToDir);
    if (!dir) {
        printf("\nCould not open directory\n");
        return 2;
    }

    while ((entry = readdir(dir)) != NULL) {
        if(entry->d_name == "." || entry->d_name == "..")
            continue;

        uint8_t tempData = 0;
        AudioData wemTemp = {0, 1, &tempData};

        printf("\n-----------------Opened new WEM file-------------");

        char* pathToFile = "";
        strcpy(pathToFile, pathToDir);

        strcat(pathToFile, "/");
        strcat(pathToFile, entry->d_name);

        if (fopen_s(&file, pathToFile, "rb") == 0)
        {
            fseek( file, 0, SEEK_END);

            long position = ftell(file);

            char* tempString = strtok(entry->d_name, ".");

            wemTemp.id = atol(tempString);
            printf("\n|\twemTemp.id: %d\t\t\t|", wemTemp.id);
            wemTemp.length = position;
            printf("\n|\twemTemp.length: %d\t\t\t|", wemTemp.length);
            wemTemp.data = malloc(wemTemp.length);
            printf("\n|\twemTemp.data: %s\t\t\t|", wemTemp.data);

            rewind(file);
            fread(wemTemp.data, wemTemp.length, 1, file);

            listTemp.length++;
            printf("\n|\tlistTemp.length: %llu\t\t\t|", listTemp.length);

            Sleep(1);

            listTemp.objects[counter] = wemTemp;
            counter++;

            fclose(file);
        }

        printf("\n-----------------Closed WEM file-----------------\n");
    }

    char* tempName;
    strcpy(tempName, wpkName);
    strcat(tempName, ".wpk");

    write_wpk_file(wemList, tempName);
}