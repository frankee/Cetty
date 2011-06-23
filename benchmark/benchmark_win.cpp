


int runApplication(const std::string& name, const std::string& param, std::string& output) {
    SECURITY_ATTRIBUTES sa;
    HANDLE hRead,hWrite;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead,&hWrite,&sa,0)) {
        return -1;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcess(name.c_str(),param.c_str(),NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) {
        return -1;
    }
    CloseHandle(hWrite);

    char buffer[4096] = {0};
    DWORD bytesRead;
    while (true) {
        if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)
            break;

        output += buffer;
    }

    return 0;
}

const int BLOCK_SIZE_ARRY[] = {32, 64, 128, 256, 512, 1024, 4096, 16384, 65536};
const int CLIENT_COUNT_ARRY[] = {1, 10, 50, 100, 500, 1000, 3000, 5000};
const int CLIENT_COUNT_ARRY_SIZE = sizeof(CLIENT_COUNT_ARRY)/sizeof(int);
const int BLOCK_SIZE_ARRY_SIZE = sizeof(CLIENT_COUNT_ARRY)/sizeof(int);


class ServerTest {
public:
    void start(int blocksize);
    void stop();
};

class ClientTest {
public:
    void start(int blocksize, int sessionCnt);
    void stop();
};

std::pair<int, int> startClient(int blocksize, int sessionCnt) {

}

void startTest(int blocksize, std::vector<std::pair<int, int> >& result) {
    result.clear();
    startServer(blocksize);
    
    for(int i = 0; i < CLIENT_COUNT_ARRY_SIZE; ++i) {
        std::pair<int, int> ret;
        ret = startClient(blocksize, CLIENT_COUNT_ARRY[i]);
        result.push_back(ret);
    }
}

int main(int argc, char* argv) {

    // run test
    fstream file("benchmark.csv");
    file << "blocksize,";
    for (int i = 0; i < CLIENT_COUNT_ARRY_SIZE; ++i) {
        file << CLIENT_COUNT_ARRY[i] << " concurrent,";
    }
    file << std::endl;

    std::vector<std::pair<int, int> > result;
    for (int i = 0; i < BLOCK_SIZE_ARRY_SIZE; ++i) {
        file << BLOCK_SIZE_ARRY[i];
        startTest(BLOCK_SIZE_ARRY[i], result);

        for (size_t j = 0; result.size(); ++j {
            file << result[i].first + result[i].second << ","
        }
        file << std::endl;
    }
    
    file.close();

    return 0;
}