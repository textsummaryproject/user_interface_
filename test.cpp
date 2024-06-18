#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <conio.h>


#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
using namespace std;

void PrintInitView() {
    cout << "\n\n\n\n";
    cout << "\t\t\t\t\t==========================================" << endl;
    cout << "\t\t\t\t\t                                          " << endl;
    cout << "\t\t\t\t\t         문 서 요 약 프 로 그 램          " << endl;
    cout << "\t\t\t\t\t                                          " << endl;
    cout << "\t\t\t\t\t==========================================" << endl;

    cout << "\n\n";
    //cout << "\t\t\t\t\t==========================================" << endl;
    cout << "\t\t\t\t\t          오픈소스개발프로젝트\n";
    cout << "\t\t\t\t\t            제작자: 뚜벅이들\n\n\n";
    //cout << "\t\t\t\t\t==========================================\n" << endl;
    // 사용자가 엔터를 누를 때까지 대기
    cout << "\t\t\t\t\t        엔터를 누르면 계속합니다...";
    cin.get();

}






// 메뉴를 보여주는 함수
void displayMenu(const vector<string>& options, int currentSelection) {
    system("cls"); // 화면 지우기

    // 콘솔 창 크기 구하기
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // 메뉴 출력
    
    cout << "사용하고자 하는 기능을 선택하세요:\n";
    int maxOptionLength = 0;
    for (const string& option : options) {
        maxOptionLength = max(maxOptionLength, static_cast<int>(option.length()));
    }

    for (const string& option : options) {
        int padding = (consoleWidth - maxOptionLength) / 2;
        cout << string(padding, ' ') << option << endl;
    }

    // 선택된 항목 표시
    cout << "\n\n\n";
    cout << "\t\t\t\t\t"<<string(consoleWidth / 2 - options[currentSelection].length() / 2, ' ') << "> " << options[currentSelection] << " <" << endl;
}


// UTF-8 문자열을 ANSI 문자열로 변환하는 함수
char* UTF8ToANSI(const char* pszCode) {
    BSTR bstrWide;
    char* pszAnsi;
    int nLength;

    nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
    bstrWide = SysAllocStringLen(NULL, nLength);

    MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

    nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
    pszAnsi = new char[nLength];

    WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
    SysFreeString(bstrWide);

    return pszAnsi;
}

// 단어를 소문자로 변환하는 함수
std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// 문자열을 단어로 분할하는 함수
std::vector<std::string> split(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// TF-IDF 계산 함수
map<string, double> computeTFIDF(const vector<string>& doc, const vector<vector<string>>& corpus) {
    map<string, double> tfidf;
    map<string, int> termFrequency;
    map<string, int> documentFrequency;
    int totalTerms = doc.size();

    // TF 계산
    for (const string& word : doc) {
        termFrequency[word]++;
    }

    // DF 계산
    for (const vector<string>& document : corpus) {
        set<string> uniqueWords(document.begin(), document.end());
        for (const string& word : uniqueWords) {
            documentFrequency[word]++;
        }
    }

    // TF-IDF 계산
    for (const auto& pair : termFrequency) {
        const string& word = pair.first;
        int tf = pair.second;
        int df = documentFrequency[word];
        double idf = log((double)corpus.size() / (df + 1));
        tfidf[word] = (tf / (double)totalTerms) * idf;
    }

    return tfidf;
}

void summarizeText(const std::string& text) {
    // 문장 분할
    istringstream iss(text);
    string line;
    vector<std::string> sentences;
    while (getline(iss, line, '.')) {
        if (!line.empty()) {
            sentences.push_back(line + ".");
        }
    }

    // 문서를 단어로 분할
    vector<vector<std::string>> corpus;
    for (const string& sentence : sentences) {
        corpus.push_back(split(toLowerCase(sentence)));
    }

    // 각 문장의 TF-IDF 계산
    vector<pair<string, double>> sentenceScores;
    for (const vector<string>& doc : corpus) {
        map<string, double> tfidf = computeTFIDF(doc, corpus);
        double score = 0.0;
        for (const auto& pair : tfidf) {
            score += pair.second;
        }
        sentenceScores.push_back(make_pair(sentences[&doc - &corpus[0]], score));
    }

    // TF-IDF 점수가 높은 순으로 문장 정렬
    sort(sentenceScores.begin(), sentenceScores.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
        return b.second > a.second;
        });

    // 요약 결과 출력 (상위 3문장)
    cout << "요약 결과:\n";
    for (size_t i = 0; i < min<size_t>(sentenceScores.size(), size_t(3)); ++i) {
        cout << sentenceScores[i].first << "\n";
    }
}

void summarizeFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "파일을 열 수 없습니다." << endl;
        return;
    }
    stringstream ss;
    ss << file.rdbuf();
    string utf8Str = ss.str(); // UTF-8 인코딩 문자열

    // UTF-8 문자열을 ANSI 문자열로 변환
    char* ansiStr = UTF8ToANSI(utf8Str.c_str());

    cout << ansiStr << endl; // 변환된 문자열 출력

    summarizeText(ansiStr);

    delete[] ansiStr; // 메모리 해제
}


int main() {
    PrintInitView();

    // 메뉴 옵션 설정
    vector<string> options = { "이미지 입력", "텍스트 입력", "문서 입력" };
    int currentSelection = 0;

    // 메뉴 선택 반복
    while (true) {
        // 메뉴 출력
        system("cls"); // 콘솔 창 지우기
        cout << "\n\n\n\n\n\n";
        cout << "\t\t\t\t\t==========================================" << endl;
        cout << "\t\t\t\t\t   사용하고자 하는 기능을 선택하세요:\n";
        cout << "\t\t\t\t\t==========================================" << endl;
        cout << "\n";
        for (int i = 0; i < options.size(); ++i) {
            if (i == currentSelection) {
                
                cout << "\t\t\t\t\t\t\t" << "> " << options[i] << " <" << endl; // 선택된 항목을 강조
            }
            else {
                cout << "\t\t\t\t\t\t\t" << options[i] << endl;
            }
        }

        int ch = _getch(); // 키 입력 받기
        if (ch == 224) { // 화살표 키 입력
            switch (_getch()) {
            case 72: // 위 화살표 키
                currentSelection = (currentSelection - 1 + options.size()) % options.size();
                break;
            case 80: // 아래 화살표 키
                currentSelection = (currentSelection + 1) % options.size();
                break;
            }
        }
        else if (ch == 13) { // 엔터 키 입력
            break;
        }
    }

    // 선택된 기능 실행
    switch (currentSelection) {
    case 0: {
        system("cls"); // 화면 지우기
        // 이미지 입력 처리
        std::string image_path = "test.png";

        // 이미지 파일 읽어오기
        cv::Mat original_image = cv::imread(image_path);

        // 이미지를 성공적으로 읽었는지 확인
        if (original_image.empty()) {
            std::cerr << "이미지를 읽을 수 없습니다." << std::endl;
            return -1;
        }

        // 이미지 크기 조정
        cv::resize(original_image, original_image, cv::Size(), 1.5, 1.5);

        // 이미지를 그레이스케일로 변환
        cv::Mat gray_image;
        cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);

        // 이미지 이진화
        cv::Mat binary_image;
        cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        // MSER 객체 생성
        cv::Ptr<cv::MSER> mser = cv::MSER::create();

        // MSER 알고리즘을 이용하여 텍스트 영역 검출
        std::vector<std::vector<cv::Point>> regions;
        std::vector<cv::Rect> rects;
        mser->detectRegions(binary_image, regions, rects);

        // 검출된 텍스트 영역에 네모박스 그리기
        for (size_t i = 0; i < rects.size(); ++i) {
            cv::rectangle(original_image, rects[i], cv::Scalar(0, 255, 0), 2);
        }

        // 텍스트 영역이 표시된 이미지 출력
        cv::imshow("텍스트 영역 검출 결과", original_image);

        // Tesseract OCR 초기화 및 텍스트 추출
        const char* input_image = "test.png";
        const char* dataPath = "C:/Program Files/Tesseract-OCR/tessdata";

        tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
        if (api->Init(dataPath, "kor")) {
            return -1;
        }

        Pix* image = pixRead(input_image);
        api->SetImage(image);

        std::string utf_text = api->GetUTF8Text();
        std::string text = UTF8ToANSI(utf_text.c_str());

        std::cout << "OCR 결과:\n" << text << std::endl;

        summarizeText(text);

        // 메모리 해제
        api->End();
        pixDestroy(&image);

        // 키 입력 대기

        cv::waitKey(0);

        //// 계속할지 종료할지 선택
        //while (true) {
        //    cout << "\n계속하려면 'a', 종료하려면 'e', 이전 메뉴로 가려면 'q'를 누르세요: ";
        //    char choice = _getch();
        //    if (choice == 'a') {
        //        // 이미지 입력 처리
        //        std::string image_path = "test.png";

        //        // 이미지 파일 읽어오기
        //        cv::Mat original_image = cv::imread(image_path);

        //        // 이미지를 성공적으로 읽었는지 확인
        //        if (original_image.empty()) {
        //            std::cerr << "이미지를 읽을 수 없습니다." << std::endl;
        //            return -1;
        //        }

        //        // 이미지 크기 조정
        //        cv::resize(original_image, original_image, cv::Size(), 1.5, 1.5);

        //        // 이미지를 그레이스케일로 변환
        //        cv::Mat gray_image;
        //        cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);

        //        // 이미지 이진화
        //        cv::Mat binary_image;
        //        cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        //        // MSER 객체 생성
        //        cv::Ptr<cv::MSER> mser = cv::MSER::create();

        //        // MSER 알고리즘을 이용하여 텍스트 영역 검출
        //        std::vector<std::vector<cv::Point>> regions;
        //        std::vector<cv::Rect> rects;
        //        mser->detectRegions(binary_image, regions, rects);

        //        // 검출된 텍스트 영역에 네모박스 그리기
        //        for (size_t i = 0; i < rects.size(); ++i) {
        //            cv::rectangle(original_image, rects[i], cv::Scalar(0, 255, 0), 2);
        //        }

        //        // 텍스트 영역이 표시된 이미지 출력
        //        cv::imshow("텍스트 영역 검출 결과", original_image);

        //        // Tesseract OCR 초기화 및 텍스트 추출
        //        const char* input_image = "test.png";
        //        const char* dataPath = "C:/Program Files/Tesseract-OCR/tessdata";

        //        tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
        //        if (api->Init(dataPath, "kor")) {
        //            return -1;
        //        }

        //        Pix* image = pixRead(input_image);
        //        api->SetImage(image);

        //        std::string utf_text = api->GetUTF8Text();
        //        std::string text = UTF8ToANSI(utf_text.c_str());

        //        std::cout << "OCR 결과:\n" << text << std::endl;

        //        summarizeText(text);

        //        // 메모리 해제
        //        api->End();
        //        pixDestroy(&image);
        //    }
        //    else if (choice == 'e') {
        //        return 0; // 종료
        //    }
        //    else if (choice == 'q') {
        //        system("cls");
        //        main(); // 이전 메뉴로 돌아가기
        //    }
        //}
        break;
    }
    case 1: {
        // 사용자로부터 텍스트 입력받기
        std::cout << "\n사용자 텍스트 입력:\n";
        std::string userInput;
        //std::cout << "\t\t\t\t\t\t\t";
        std::getline(std::cin, userInput);
        cout << endl;
        summarizeText(userInput);
        // 계속할지 종료할지 선택
        while (true) {
            cout << "\n계속하려면 'a', 종료하려면 'e', 이전 메뉴로 가려면 'q'를 누르세요: ";
            char choice = _getch();
            if (choice == 'a') {
                std::cout << "\n사용자 텍스트 입력:\n";
                std::string userInput;
                std::getline(std::cin, userInput);
                cout << endl;
                summarizeText(userInput);
            }
            else if (choice == 'e') {
                return 0; // 종료
            }
            else if (choice == 'q') {
                system("cls");
                main(); // 이전 메뉴로 돌아가기
            }
        }
        break;
    }
    case 2: {
        // 파일 경로 입력 받아서 요약
        cout << "\n파일 경로 입력:\n";
        string filePath;
        getline(cin, filePath);
        cout << endl;
        summarizeFile(filePath);
        // 계속할지 종료할지 선택
        while (true) {
            cout << "\n계속하려면 'a', 종료하려면 'e', 이전 메뉴로 가려면 'q'를 누르세요: ";
            char choice = _getch();
            if (choice == 'a') {
                cout << "\n파일 경로 입력:\n";
                string filePath;
                getline(cin, filePath);
                cout << endl;
                summarizeFile(filePath);
            }
            else if (choice == 'e') {
                return 0; // 종료
            }
            else if (choice == 'q') {
                system("cls");
                main(); // 이전 메뉴로 돌아가기
            }
        }
        break;
    }
    default:
        std::cerr << "잘못된 입력입니다." << std::endl;
        return -1;
    }
    return 0;
}

