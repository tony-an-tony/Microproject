#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50

struct Event {
    int id;
    char name[MAX_NAME_LEN];
    int totaltickets;
    int soldtickets;
    int remainingtickets;
    int day,month,year;
} Event;

void addevent();
void purchaseticket();
void viewevents();
void menu();

int main() {
    while (1) {
        menu();
    }
    return 0;
}

void menu() {
    int choice;
    printf("\n--- Event Ticketing System ---\n");
    printf("1. Add Event\n");
    printf("2. Purchase Ticket\n");
    printf("3. View Events\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            addevent();
            break;
        case 2:
            purchaseticket();
            break;
        case 3:
            viewevents();
            break;
        case 4:
            printf("Exiting...\n");
            exit(0);
        default:
            printf("Invalid choice! Please try again.\n");
    }
}

void addevent() {
    FILE *fp;
    struct Event event;
    fp = fopen("events.dat", "ab");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter event ID: ");
    scanf("%d", &event.id);
    getchar();
    printf("Enter event name: ");
    fgets(event.name, MAX_NAME_LEN, stdin);
    size_t len = strlen(event.name);
    if (len > 0 && event.name[len - 1] == '\n'){
    event.name[len - 1] = '\0';
    }
    printf("Enter total number of tickets: ");
    scanf("%d", &event.totaltickets);
    event.soldtickets = 0;
    event.remainingtickets = event.totaltickets;
    printf("Enter event date (dd/mm/yyyy): ");
    char edate[15];
    scanf("%11s", edate);
    int day, month, year;
    sscanf(edate, "%d/%d/%d", &day, &month, &year);
    event.day = day;
    event.month = month;
    event.year = year;

    if (fwrite(&event, sizeof(Event), 1, fp) != 1) {
        printf("Error writing event to file!\n");
    }

    if (fclose(fp) != 0) {
        printf("Error closing file!\n");
    }

    printf("Event added successfully.\n");
}


void purchaseticket() {
    FILE *fp;
    struct Event event;
    int id, found = 0;
    fp = fopen("events.dat", "r+b");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter event ID to purchase ticket: ");
    scanf("%d", &id);

    while (fread(&event, sizeof(Event), 1, fp)) {
        if (event.id == id) {
            time_t currentTime;
            struct tm *localTime;
            currentTime = time(NULL);
            localTime = localtime(&currentTime);

            if (event.year < localTime->tm_year + 1900 ||
                (event.year == localTime->tm_year + 1900 &&
                 (event.month < localTime->tm_mon + 1 ||
                  (event.month == localTime->tm_mon + 1 && event.day < localTime->tm_mday)))) {
                printf("%s  has expired.\n", event.name);
                goto closing;
            }

            if (event.soldtickets < event.totaltickets) {
                event.soldtickets += 1;
                event.remainingtickets = event.totaltickets - event.soldtickets;
                fseek(fp, -sizeof(Event), SEEK_CUR);
                fwrite(&event, sizeof(Event), 1, fp);
                found = 1;
                printf("Ticket purchased for %s.\n", event.name);
            } else {
                printf("No tickets available for %s.\n", event.name);
            }
            goto closing;
        }
    }

    if (!found) {
        printf("Event with ID %d not found.\n", id);
    }
closing:
    fclose(fp);
}

void viewevents() {
    FILE *fp;
    struct Event event;
    fp = fopen("events.dat", "rb");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\n--------------------------------------- EVENT LIST ---------------------------------------\n");
    printf("%s\t %s\t %s\t      %s\t %s\t %s\n", "Event ID", "Name", "Date", "Total Tickets", "Sold Tickets", "Remaining Tickets");
    printf("------------------------------------------------------------------------------------------\n");

    while (fread(&event, sizeof(Event), 1, fp)) {
        printf("%5d\t %12s\t %02d/%02d/%02d\t %3d\t %15d\t %10d",
               event.id, event.name, event.day, event.month, event.year,
               event.totaltickets, event.soldtickets, event.remainingtickets);
        printf("\n");
    }

    fclose(fp);
}