#include "bsp_log.h"

/* Example log output function to save logs to a file */
void log_to_file(const char *log)
{
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

int main(void)
{
    /* Set log output to file */
    // set_log_output(log_to_file);

    log_printf(LOG_INFO, "This is an info message.");
    log_printf(LOG_DEBUG, "This is a debug message.");
    log_printf(LOG_WARN, "This is a warning message.");
    log_printf(LOG_ERROR, "This is an error message.");

    return 0;
}