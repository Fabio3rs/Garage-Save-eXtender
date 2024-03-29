/*****************************************************************************************************
 *		GUITAR++
 *		PROGRAMADO POR FÁBIO
 *		BMS - Brazilian Modding Studio -
 *http://brmodstudio.forumeiros.com
 *****************************************************************************************************/
#include "CLog.h"
#include <cstdio>
#include <ctime>

auto CLog::log() -> CLog & {
    static CLog Log("gsx.log");
    return Log;
}

CLog::CLog(const std::string &NameOfFile) {
    Finished = false;
    FileName = NameOfFile;
    LogFile.rdbuf()->pubsetbuf(0, 0);
    LogFile.open(NameOfFile, std::ios::in | std::ios::out | std::ios::trunc);

    if (!LogFile.good())
        throw CLogException("Impossivel criar ou abrir o arquivo de log");

    std::string LogContents;
    LogContents += "***********************************************************"
                   "******************\n";
    LogContents +=
        std::string("* GSX - Garage Save eXtender compilation date/time: ") +
        __DATE__ + " " + __TIME__;
    LogContents += "\n*********************************************************"
                   "********************\n";
    LogContents += "***********************************************************"
                   "******************\n* Log started at: ";
    LogContents += GetDateAndTime();
    LogContents += "\n*********************************************************"
                   "********************\n\n";
    LogFile.write(LogContents.c_str(), LogContents.size());
}

CLog::~CLog() noexcept { FinishLog(); }

auto CLog::GetDateAndTime() -> std::string {
    std::time_t tt =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string str = std::string(ctime(&tt));
    if (str[str.size() - 1] == '\n') {
        str.resize(str.size() - 1);
    }
    return str;
}

void CLog::AddToLog(const std::string &Text) {
    std::string Temp;
    Temp += GetDateAndTime();
    Temp += ": ";
    Temp += Text;
    Temp += "\n";
    LogFile.write(Temp.c_str(), Temp.size());
    // LogFile << Temp;
}

void CLog::FinishLog() {
    std::string LogContents;
    LogContents += "\n*********************************************************"
                   "********************\n* Log Finished at: ";
    LogContents += GetDateAndTime();
    LogContents += "\n*********************************************************"
                   "********************\n";
    LogFile.clear();
    LogFile.write(LogContents.c_str(), LogContents.size());
    LogFile.flush();
    Finished = true;
}

void CLog::SaveBuffer() { LogFile.flush(); }