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
    cout << "\t\t\t\t\t         �� �� �� �� �� �� �� ��          " << endl;
    cout << "\t\t\t\t\t                                          " << endl;
    cout << "\t\t\t\t\t==========================================" << endl;

    cout << "\n\n";
    //cout << "\t\t\t\t\t==========================================" << endl;
    cout << "\t\t\t\t\t          ���¼ҽ�����������Ʈ\n";
    cout << "\t\t\t\t\t            ������: �ѹ��̵�\n\n\n";
    //cout << "\t\t\t\t\t==========================================\n" << endl;
    // ����ڰ� ���͸� ���� ������ ���
    cout << "\t\t\t\t\t        ���͸� ������ ����մϴ�...";
    cin.get();

}






// �޴��� �����ִ� �Լ�
void displayMenu(const vector<string>& options, int currentSelection) {
    system("cls"); // ȭ�� �����

    // �ܼ� â ũ�� ���ϱ�
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // �޴� ���
    
    cout << "����ϰ��� �ϴ� ����� �����ϼ���:\n";
    int maxOptionLength = 0;
    for (const string& option : options) {
        maxOptionLength = max(maxOptionLength, static_cast<int>(option.length()));
    }

    for (const string& option : options) {
        int padding = (consoleWidth - maxOptionLength) / 2;
        cout << string(padding, ' ') << option << endl;
    }

    // ���õ� �׸� ǥ��
    cout << "\n\n\n";
    cout << "\t\t\t\t\t"<<string(consoleWidth / 2 - options[currentSelection].length() / 2, ' ') << "> " << options[currentSelection] << " <" << endl;
}


// UTF-8 ���ڿ��� ANSI ���ڿ��� ��ȯ�ϴ� �Լ�
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

// �ܾ �ҹ��ڷ� ��ȯ�ϴ� �Լ�
std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// ���ڿ��� �ܾ�� �����ϴ� �Լ�
std::vector<std::string> split(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// TF-IDF ��� �Լ�
map<string, double> computeTFIDF(const vector<string>& doc, const vector<vector<string>>& corpus) {
    map<string, double> tfidf;
    map<string, int> termFrequency;
    map<string, int> documentFrequency;
    int totalTerms = doc.size();

    // TF ���
    for (const string& word : doc) {
        termFrequency[word]++;
    }

    // DF ���
    for (const vector<string>& document : corpus) {
        set<string> uniqueWords(document.begin(), document.end());
        for (const string& word : uniqueWords) {
            documentFrequency[word]++;
        }
    }

    // TF-IDF ���
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
    // ���� ����
    istringstream iss(text);
    string line;
    vector<std::string> sentences;
    while (getline(iss, line, '.')) {
        if (!line.empty()) {
            sentences.push_back(line + ".");
        }
    }

    // ������ �ܾ�� ����
    vector<vector<std::string>> corpus;
    for (const string& sentence : sentences) {
        corpus.push_back(split(toLowerCase(sentence)));
    }

    // �� ������ TF-IDF ���
    vector<pair<string, double>> sentenceScores;
    for (const vector<string>& doc : corpus) {
        map<string, double> tfidf = computeTFIDF(doc, corpus);
        double score = 0.0;
        for (const auto& pair : tfidf) {
            score += pair.second;
        }
        sentenceScores.push_back(make_pair(sentences[&doc - &corpus[0]], score));
    }

    // TF-IDF ������ ���� ������ ���� ����
    sort(sentenceScores.begin(), sentenceScores.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
        return b.second > a.second;
        });

    // ��� ��� ��� (���� 3����)
    cout << "��� ���:\n";
    for (size_t i = 0; i < min<size_t>(sentenceScores.size(), size_t(3)); ++i) {
        cout << sentenceScores[i].first << "\n";
    }
}

void summarizeFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "������ �� �� �����ϴ�." << endl;
        return;
    }
    stringstream ss;
    ss << file.rdbuf();
    string utf8Str = ss.str(); // UTF-8 ���ڵ� ���ڿ�

    // UTF-8 ���ڿ��� ANSI ���ڿ��� ��ȯ
    char* ansiStr = UTF8ToANSI(utf8Str.c_str());

    cout << ansiStr << endl; // ��ȯ�� ���ڿ� ���

    summarizeText(ansiStr);

    delete[] ansiStr; // �޸� ����
}


int main() {
    PrintInitView();

    // �޴� �ɼ� ����
    vector<string> options = { "�̹��� �Է�", "�ؽ�Ʈ �Է�", "���� �Է�" };
    int currentSelection = 0;

    // �޴� ���� �ݺ�
    while (true) {
        // �޴� ���
        system("cls"); // �ܼ� â �����
        cout << "\n\n\n\n\n\n";
        cout << "\t\t\t\t\t==========================================" << endl;
        cout << "\t\t\t\t\t   ����ϰ��� �ϴ� ����� �����ϼ���:\n";
        cout << "\t\t\t\t\t==========================================" << endl;
        cout << "\n";
        for (int i = 0; i < options.size(); ++i) {
            if (i == currentSelection) {
                
                cout << "\t\t\t\t\t\t\t" << "> " << options[i] << " <" << endl; // ���õ� �׸��� ����
            }
            else {
                cout << "\t\t\t\t\t\t\t" << options[i] << endl;
            }
        }

        int ch = _getch(); // Ű �Է� �ޱ�
        if (ch == 224) { // ȭ��ǥ Ű �Է�
            switch (_getch()) {
            case 72: // �� ȭ��ǥ Ű
                currentSelection = (currentSelection - 1 + options.size()) % options.size();
                break;
            case 80: // �Ʒ� ȭ��ǥ Ű
                currentSelection = (currentSelection + 1) % options.size();
                break;
            }
        }
        else if (ch == 13) { // ���� Ű �Է�
            break;
        }
    }

    // ���õ� ��� ����
    switch (currentSelection) {
    case 0: {
        system("cls"); // ȭ�� �����
        // �̹��� �Է� ó��
        std::string image_path = "test.png";

        // �̹��� ���� �о����
        cv::Mat original_image = cv::imread(image_path);

        // �̹����� ���������� �о����� Ȯ��
        if (original_image.empty()) {
            std::cerr << "�̹����� ���� �� �����ϴ�." << std::endl;
            return -1;
        }

        // �̹��� ũ�� ����
        cv::resize(original_image, original_image, cv::Size(), 1.5, 1.5);

        // �̹����� �׷��̽����Ϸ� ��ȯ
        cv::Mat gray_image;
        cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);

        // �̹��� ����ȭ
        cv::Mat binary_image;
        cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        // MSER ��ü ����
        cv::Ptr<cv::MSER> mser = cv::MSER::create();

        // MSER �˰����� �̿��Ͽ� �ؽ�Ʈ ���� ����
        std::vector<std::vector<cv::Point>> regions;
        std::vector<cv::Rect> rects;
        mser->detectRegions(binary_image, regions, rects);

        // ����� �ؽ�Ʈ ������ �׸�ڽ� �׸���
        for (size_t i = 0; i < rects.size(); ++i) {
            cv::rectangle(original_image, rects[i], cv::Scalar(0, 255, 0), 2);
        }

        // �ؽ�Ʈ ������ ǥ�õ� �̹��� ���
        cv::imshow("�ؽ�Ʈ ���� ���� ���", original_image);

        // Tesseract OCR �ʱ�ȭ �� �ؽ�Ʈ ����
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

        std::cout << "OCR ���:\n" << text << std::endl;

        summarizeText(text);

        // �޸� ����
        api->End();
        pixDestroy(&image);

        // Ű �Է� ���

        cv::waitKey(0);

        //// ������� �������� ����
        //while (true) {
        //    cout << "\n����Ϸ��� 'a', �����Ϸ��� 'e', ���� �޴��� ������ 'q'�� ��������: ";
        //    char choice = _getch();
        //    if (choice == 'a') {
        //        // �̹��� �Է� ó��
        //        std::string image_path = "test.png";

        //        // �̹��� ���� �о����
        //        cv::Mat original_image = cv::imread(image_path);

        //        // �̹����� ���������� �о����� Ȯ��
        //        if (original_image.empty()) {
        //            std::cerr << "�̹����� ���� �� �����ϴ�." << std::endl;
        //            return -1;
        //        }

        //        // �̹��� ũ�� ����
        //        cv::resize(original_image, original_image, cv::Size(), 1.5, 1.5);

        //        // �̹����� �׷��̽����Ϸ� ��ȯ
        //        cv::Mat gray_image;
        //        cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);

        //        // �̹��� ����ȭ
        //        cv::Mat binary_image;
        //        cv::threshold(gray_image, binary_image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

        //        // MSER ��ü ����
        //        cv::Ptr<cv::MSER> mser = cv::MSER::create();

        //        // MSER �˰����� �̿��Ͽ� �ؽ�Ʈ ���� ����
        //        std::vector<std::vector<cv::Point>> regions;
        //        std::vector<cv::Rect> rects;
        //        mser->detectRegions(binary_image, regions, rects);

        //        // ����� �ؽ�Ʈ ������ �׸�ڽ� �׸���
        //        for (size_t i = 0; i < rects.size(); ++i) {
        //            cv::rectangle(original_image, rects[i], cv::Scalar(0, 255, 0), 2);
        //        }

        //        // �ؽ�Ʈ ������ ǥ�õ� �̹��� ���
        //        cv::imshow("�ؽ�Ʈ ���� ���� ���", original_image);

        //        // Tesseract OCR �ʱ�ȭ �� �ؽ�Ʈ ����
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

        //        std::cout << "OCR ���:\n" << text << std::endl;

        //        summarizeText(text);

        //        // �޸� ����
        //        api->End();
        //        pixDestroy(&image);
        //    }
        //    else if (choice == 'e') {
        //        return 0; // ����
        //    }
        //    else if (choice == 'q') {
        //        system("cls");
        //        main(); // ���� �޴��� ���ư���
        //    }
        //}
        break;
    }
    case 1: {
        // ����ڷκ��� �ؽ�Ʈ �Է¹ޱ�
        std::cout << "\n����� �ؽ�Ʈ �Է�:\n";
        std::string userInput;
        //std::cout << "\t\t\t\t\t\t\t";
        std::getline(std::cin, userInput);
        cout << endl;
        summarizeText(userInput);
        // ������� �������� ����
        while (true) {
            cout << "\n����Ϸ��� 'a', �����Ϸ��� 'e', ���� �޴��� ������ 'q'�� ��������: ";
            char choice = _getch();
            if (choice == 'a') {
                std::cout << "\n����� �ؽ�Ʈ �Է�:\n";
                std::string userInput;
                std::getline(std::cin, userInput);
                cout << endl;
                summarizeText(userInput);
            }
            else if (choice == 'e') {
                return 0; // ����
            }
            else if (choice == 'q') {
                system("cls");
                main(); // ���� �޴��� ���ư���
            }
        }
        break;
    }
    case 2: {
        // ���� ��� �Է� �޾Ƽ� ���
        cout << "\n���� ��� �Է�:\n";
        string filePath;
        getline(cin, filePath);
        cout << endl;
        summarizeFile(filePath);
        // ������� �������� ����
        while (true) {
            cout << "\n����Ϸ��� 'a', �����Ϸ��� 'e', ���� �޴��� ������ 'q'�� ��������: ";
            char choice = _getch();
            if (choice == 'a') {
                cout << "\n���� ��� �Է�:\n";
                string filePath;
                getline(cin, filePath);
                cout << endl;
                summarizeFile(filePath);
            }
            else if (choice == 'e') {
                return 0; // ����
            }
            else if (choice == 'q') {
                system("cls");
                main(); // ���� �޴��� ���ư���
            }
        }
        break;
    }
    default:
        std::cerr << "�߸��� �Է��Դϴ�." << std::endl;
        return -1;
    }
    return 0;
}

