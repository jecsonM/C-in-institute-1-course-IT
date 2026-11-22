#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>

#define MAX_RECORDS 100

typedef struct {
    int  id;
    char author[32];
    char title[64];
    int  year;
} Record;

typedef struct {
    Record items[MAX_RECORDS];
    int    count;
} DB;


int find_record(const DB* db, int key) {
    for (int i = 0; i < db->count; i++)
        if (db->items[i].id == key) return i;
    return -1;
}

void print_record(const Record* r) {
    printf("%d %s %s %d\n", r->id, r->author, r->title, r->year);
}

void print_menu(void) {
    printf("\n=== База книг ===\n");
    printf("  1 - добавить запись\n");
    printf("  2 - показать все записи\n");
    printf("  3 - найти по ключу\n");
    printf("  4 - изменить запись\n");
    printf("  5 - удалить запись\n");
    printf("  6 - упорядочить записи\n");
    printf("  0 - сохранить и выйти\n");
}


void load_db(DB* db) {
    db->count = 0;
    FILE* f = fopen("records.txt", "r");
    if (!f) {
        if (errno == ENOENT) return;         
        perror("records.txt");
        exit(EXIT_FAILURE);
    }

    int n;
    if (fscanf_s(f, "%d", &n) != 1 || n < 0 || n > MAX_RECORDS) {
        fprintf(stderr, "Ошибка чтения: неверное число записей\n");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    

    for (int i = 0; i < n; i++) {
        Record r;
        if (fscanf_s(f, "%d %31s %63s %d",
            &r.id,
            r.author, (unsigned)sizeof(r.author),
            r.title, (unsigned)sizeof(r.title),
            &r.year) != 4) {
            fprintf(stderr, "Ошибка чтения: неполная запись\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }
        db->items[i] = r;
    }
    db->count = n;
    fclose(f);
}


int save_db(const DB* db) {
    FILE* f = fopen("records.txt", "w");
    if (!f) { perror("records.txt"); return 0; }

    fprintf(f, "%d\n", db->count);
    for (int i = 0; i < db->count; i++) {
        fprintf(f, "%d %s %s %d\n",
            db->items[i].id, db->items[i].author,
            db->items[i].title, db->items[i].year);
    }
    fclose(f);
    return 1;
}


void sort_db(DB* db) {
    for (int i = 1; i < db->count; i++) {
        Record key = db->items[i];
        int j = i - 1;
        while (j >= 0 &&
            (db->items[j].year < key.year ||
                (db->items[j].year == key.year && db->items[j].id > key.id))) {
            db->items[j + 1] = db->items[j];
            j--;
        }
        db->items[j + 1] = key;
    }
}


int main(void) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    DB db;
    load_db(&db);

    int cmd;
    for (;;) {

        print_menu();
        printf("Команда: ");
        if (scanf_s("%d", &cmd) != 1) break;

        if (cmd == 1) {                          //add
            Record r;
            printf("id author title year: ");
            if (scanf_s("%d %31s %63s %d",
                &r.id,
                r.author, (unsigned)sizeof(r.author),
                r.title, (unsigned)sizeof(r.title),
                &r.year) != 4) break;

            if (db.count >= MAX_RECORDS) {
                printf_s("Ошибка: база заполнена\n");
            }
            else if (find_record(&db, r.id) != -1) {
                printf_s("Ошибка: запись с таким ключом уже существует\n");
            }
            else {
                db.items[db.count++] = r;
            }

        }
        else if (cmd == 2) {                   //showALl
            if (db.count == 0) printf("База пуста\n");
            else for (int i = 0; i < db.count; i++) print_record(&db.items[i]);

        }
        else if (cmd == 3) {                   //find
            int key;
            printf("Ключ: ");
            if (scanf_s("%d", &key) != 1) break;
            int i = find_record(&db, key);
            if (i < 0) printf("Не найдено\n");
            else       print_record(&db.items[i]);

        }
        else if (cmd == 4) {                   //edit
            int key;
            printf_s("Ключ: ");
            if (scanf_s("%d", &key) != 1) break;
            int i = find_record(&db, key);
            if (i < 0) {
                printf_s("Не найдено\n");
            }
            else {
                print_record(&db.items[i]);

                printf_s("author title year: ");
                Record r = db.items[i];          


                if (scanf_s("%31s %63s %d",
                    r.author, (unsigned)sizeof(r.author),
                    r.title, (unsigned)sizeof(r.title),
                    &r.year) != 3) break;
                db.items[i] = r;
            }

        }
        else if (cmd == 5) {                   //delete
            int key;
            printf_s("Ключ: ");
            if (scanf_s("%d", &key) != 1) break;
            int i = find_record(&db, key);
            if (i < 0) {
                printf_s("Не найдено\n");
            }
            else {
                for (int j = i; j < db.count - 1; j++)
                    db.items[j] = db.items[j + 1];
                db.count--;
            }

        }
        else if (cmd == 6) {                   //sort
            sort_db(&db);

        }
        else if (cmd == 0) {                   //saveAndExit
            if (!save_db(&db)) {
                fprintf_s(stderr, "Не удалось сохранить базу\n");
                return 1;
            }
            break;
        }
    }
    return 0;
}