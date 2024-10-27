#include "prioque.h"
#include <regex.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    const char* fileName = argv[1]; //grab file name from terminal input
    FILE* fp = fopen(fileName, 'r'); //create a file pointer to the file
    char currentLine[24];
    const char delimiter[1] = '\t'; //all the values are seperated by a horizontal tab

    Queue* to_be_queued;
    init_queue(to_be_queued, 5*sizeof(int), true, NULL, true); 


    // Read all the Info From the File
    if (fp != NULL)
    {
        while (fgets(currentLine, sizeof(currentLine), fp)) //reads the current line of the file into currentLine
        {
            int TIME = atoi(strtok(currentLine, delimiter)); //strtok grabs everything until a \t is met, atoi converts the value into an integer
            int PID = atoi(strtok(currentLine, delimiter));
            int RUN = atoi(strtok(currentLine, delimiter));
            int IO = atoi(strtok(currentLine, delimiter));
            int REPEAT = atoi(strtok(currentLine, delimiter));
            add_to_queue(to_be_queued, (TIME, PID, RUN, IO, REPEAT), TIME);
            bzero(currentLine); //reset currentLine after we get what we need, just in case
        }
        fclose(fp);
    }
    else
    {
        printf("Unable to Open %s: Exiting\n", fileName);
        return 1;
    }

    /////////////////////////////////////////////////////////////////
    //                      DEBUG CODE BELOW                       //
    for (int i = 0; i < to_be_queued->queuelength; i++)
    {
        pritnf(to_be_queued[i]);
    }
    /////////////////////////////////////////////////////////////////

    int clock = 0; // set the clock to 0
    /* just an outline [DELETE WHEN FINISHED]
        while (!empty_queue(q1) || !empty_queue(q2) || !empty_queue(q3) || !empty_queue(q4) || !empty_queue(to_be_queued))
        {

        }
    */
}