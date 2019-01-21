#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<deque>
#include<string>
#include<vector>


using namespace std;

deque<string> DequeOfStringsFromInputFile;
deque<vector<int>> DequeOfSortedLines;

condition_variable cv_EmptyDequeOfStringsFromInputFile;
condition_variable cv_FullDequeOfStringsFromInputFile;
condition_variable cv_EmptyDequeOfSortedLines;
condition_variable cv_FullDequeOfSortedLines;

mutex mu_DequeOfStringsFromInputFile;
mutex mu_DequeOfSortedLines;



/*
 * standard counting Sort implementation
 * http://opendatastructures.org/ods-cpp/11_2_Counting_Sort_Radix_So.html
*/
vector<int>  countingSort(vector<int> &a, int k) {
  array<int> c(k, 0);
  for (int i = 0; i < a.length; i++)
    c[a[i]]++;
  for (int i = 1; i < k; i++)
    c[i] += c[i-1];
  array<int> b(a.length);
  for (int i = a.length-1; i >= 0; i--)
    b[--c[a[i]]] = a[i];
  a = b;
}


void fu_ProcessInputFile(string& pathToInputFile)
{
    ifstream InputFile(pathToInputFile);
    for (string CurrentLine;getline(InputFile,CurrentLine);)
    {
        unique_lock<mutex> lock_DequeOfStringsFromInputFile(mu_DequeOfStringsFromInputFile);
        while(DequeOfStringsFromInputFile.size()>= DequeOfStringsFromInputFile.max_size())
            cv_FullDequeOfStringsFromInputFile.wait(lock_DequeOfStringsFromInputFile);
        DequeOfStringsFromInputFile.push_back(CurrentLine);
        lock_DequeOfStringsFromInputFile.unlock();
        cv_EmptyDequeOfStringsFromInputFile.notify_one();
    }
}

void fu_ParseAndSortLine(string& sortingAlgorithm)
{
    string LineToParseAndSort;// default value is an empty string ("")
    do{
        unique_lock<mutex> lock_DequeOfStringsFromInputFile(mu_DequeOfStringsFromInputFile);
        while(DequeOfStringsFromInputFile.empty())
            cv_EmptyDequeOfStringsFromInputFile.wait(lock_DequeOfStringsFromInputFile);
        LineToParseAndSort = DequeOfStringsFromInputFile.pop_front();
        lock_DequeOfStringsFromInputFile.unlock();
        cv_FullDequeOfStringsFromInputFile.notify_one();
        /*
         * parsing of the LineToParseAndSort
         */
        if(LineToParseAndSort.empty())
        {
            /*
             * nothing to parse.end of the DequeOfStringsFromInputFile.
             * insert empty string into DequeOfSortedLines as a symbol to end of input
             * quit from the current loop
             */
        }
        else
        {
            /*
             * the line is not empty.
             * parse,sort and insert the sorted line into DequeOfSortedLines
             */

            //TODO: call parsing function
            string ParsedLine = /* call to parsing function will return parsed line*/;
            //TODO: call one of two sorting functions depends on sortingAlgorithm value
            string SortedLine = /* call to sorting function will return sorted line*/;
            unique_lock<mutex> lock_DequeOfSortedLines(mu_DequeOfSortedLines);
            while(DequeOfSortedLines.size()>= DequeOfSortedLines.max_size())
                cv_FullDequeOfSortedLines.wait(lock_DequeOfSortedLines);
            DequeOfSortedLines.push_back(SortedLine);
            lock_DequeOfSortedLines.unlock();
            cv_EmptyDequeOfSortedLines.notify_one();
        }

    }while(LineToParseAndSort!= "");
}


void fu_WriteLineToOutputFile()
{
    /* TODO:open output file.be aware of exception between open and closing =>
       write a class that will have a closing file part in it's destructror
     */
    while (DequeOfSortedLines.front()!="")
    {
        unique_lock<mutex> lock_DequeOfSortedLines(mu_DequeOfSortedLines);
        while(DequeOfSortedLines.empty())
            cv_EmptyDequeOfSortedLines.wait(lock_DequeOfSortedLines);
        /* write the line to the file*/
        DequeOfSortedLines.pop_front();
        lock_DequeOfSortedLines.unlock();
        cv_FullDequeOfSortedLines.notify_one();
    }

}



int main(int argc,char* argv[])
{
    string pathToInputFile(argv[1]);
    string pathToOutputFile(argv[2]);
    string sortingAlgorithm(argv[3]);
    thread thread_InputFileProcessing(fu_ProcessInputFile,pathToInputFile);
    cout << "InputFileProcessing Thread started" << endl;

    thread thread_LineParsingAndSorting(fu_ParseAndSortLine,sortingAlgorithm);
    cout << "LineParsingAndSorting Thread started" << endl;


    thread thread_WritingLineToOutputFile(fu_WriteLineToOutputFile,pathToOutputFile);
    cout << "WritingLineToOutputFile Thread started" << endl;

    thread_InputFileProcessing.join();
    thread_LineParsingAndSorting.join();
    thread_WritingLineToOutputFile.join();
    return 0;
}
