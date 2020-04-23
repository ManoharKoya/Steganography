
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

vector<bool> ConvertBin(string& s) {
    vector<bool> res;
    for (char& c : s) {
        int asc = c; 
        // ascii val < 128 < 8 bits.
        // Divide each char into 8 bits - and store accordingly.
        for (int i = 0; i < 8; ++i) res.push_back((asc & (1 << i)));
    }
    assert(res.size() == 8 * s.size());
    return res;
}

void test() {
    Mat img = imread("dog.jpg", 1);
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            Vec3b& color = img.at<Vec3b>(i, j);
            for (int ii = 0; ii < 3; ++ii) {
                int p = color[ii];
                cout << p << ' ';
                for (int jj = 0; jj < 8; ++jj) {
                    cout << bool(p & (1 << jj));
                } cout << '\n';
            }
        }
    }
    return;
}
string Bool_Str(vector<bool> &v) {
    assert(v.size()%8 ==0);
    string res = "";
    for (int i = 0; i < v.size(); i+=8) {
        int c = 0;
        for (int j = 0; j < 8; ++j) {
            c += (v[i + j] * (1 << j));
        }
        res += char(c);
    }
    return res;
}
vector<bool> LSB_deStegno(Mat img) {
    vector<bool> res;
    // bool msg size is in firts 8 pixels --> 16 lsb's
    int sz = 0, pxct = 0, cols = img.cols, rows = img.rows, cur = 0;
    for (int i = 0; i < 16; i+=2) {
        int c = (pxct / 3) / rows, r = (pxct / 3) % rows, mod = pxct % 3;
        Vec3b& color = img.at<Vec3b>(c, r);
        int prev = color[mod];
        //cout << prev << '\n';
        //cout << bool(prev & 1) << ' ' << bool(prev & 2) << '\n';
        sz += ( bool(prev&1) * (1 << i)), sz += ( bool(prev&2) * (1 << (i + 1)));
        ++pxct;
    }
    sz *= 8;
    //cout << sz << '\n'; exit(0);
    for (int i = 0; i < sz; i+=2) {
        int c = (pxct / 3) / rows, r = (pxct / 3) % rows, mod = pxct % 3;
        Vec3b& color = img.at<Vec3b>(c, r);
        int prev = color[mod];
        res.push_back(bool(prev & 1));
        res.push_back(bool(prev & 2));
        ++pxct;
    }
    // for (bool i : res) cout << i; cout << '\n';
    return res;
}
void Show_lsb_image(Mat img, Mat Img) {
    Mat_<Vec3b> newLSB = Img.clone(), oldLSB = img.clone();
    for (auto& pixel : newLSB) {
        pixel[0] &= 3; pixel[0] *= 62;
        pixel[1] &= 3; pixel[1] *= 62;
        pixel[2] &= 3; pixel[2] *= 62;
    }
    for (auto& pixel : oldLSB) {
        pixel[0] &= 3; pixel[0] *= 62;
        pixel[1] &= 3; pixel[1] *= 62;
        pixel[2] &= 3; pixel[2] *= 62;
    }
    Mat newLSB1, oldLSB1;
    cvtColor(newLSB, newLSB1, COLOR_RGB2GRAY);
    cvtColor(oldLSB, oldLSB1, COLOR_RGB2GRAY);
    imshow(" Only LSB new : ", newLSB1);
    imshow(" Only LSB old : ", oldLSB1);
    waitKey(0);
    return;
}
Mat LSB_Stegno(Mat img_org, vector<bool>& Msg) {
    // each pixel can store 2 bits --> 
    // 4 pixels can store one cahracter -->
    // first 16 bits --> 8 pixels pixels will store the 'tot.no of chars stored'.
    // (tot.pixels - 16) / 4 >= tot.number of chars.

    int total_pixels = img_org.cols * img_org.rows;
    if ( (total_pixels - 16) < (Msg.size() / 2)) {
        cout << "msg is too bit to store in image..\n"; exit(0);
    } 

    Mat img = img_org.clone();
   
    //Show_lsb_image(img_org, img);
    int ch_num = int(Msg.size()) / 8, pxct = 0, cols = img.cols, rows = img.rows;
    for (int ct = 0; ct < 16; ct+=2) {
        bool Bit1 = (ch_num & (1 << ct)), Bit2 = (ch_num & (1 << (ct+1)));
        int c = (pxct / 3) / rows, r = (pxct / 3) % rows, mod = pxct % 3;
        Vec3b& color = img.at<Vec3b>(c, r);
        int val = 0, prev = color[mod];
        val += (Bit1 * 1), val += (Bit2 * 2);
        for (int i = 2; i < 8; ++i) {
            val += (prev & (1 << i));
        }
        color[mod] = val;
        img.at<Vec3b>(c, r) = color;
        pxct++;
    }
    // string of bools into imag.
    for (int i = 0; i < Msg.size(); i+=2) {
        bool Bit1 = Msg[i], Bit2 = Msg[i + 1];
        int c = (pxct / 3) / rows, r = (pxct / 3) % rows, mod = pxct % 3;
        Vec3b& color = img.at<Vec3b>(c, r);
        int val = 0, prev = color[mod];
        val += (Bit1 * 1), val += (Bit2 * 2);
        for (int j = 2; j < 8; ++j) {
            val += (prev & (1 << j));
        }
        color[mod] = val;
        img.at<Vec3b>(c, r) = color;
        pxct++;
    }
    cout << "Done ..\n";
    Mat res;
    hconcat(img, img_org, res);
    imshow("res : ", res);
    waitKey(0);
    Show_lsb_image(img_org, img);
    return img;
}

void LSB_Driver() {
    string msg = "", imgAdd, tst, fileAdd;
    cout << "File to read as message : "; cin >> fileAdd;
    ifstream myfile(fileAdd);
    if (myfile.is_open()) {
        while (getline(myfile, tst))
            msg += tst;
        myfile.close();
    }
    // cout << "Message to encrypt in image : "; cin >> msg;
    vector<bool> binaryMsg = ConvertBin(msg);
    // for (bool i : binaryMsg) cout << i; cout << '\n';
    Mat img;
    cout << "Image adderess (or) image filename : "; cin >> imgAdd;
    img = imread(imgAdd, 1);
    Mat res = LSB_Stegno(img, binaryMsg);
    vector<bool> de = LSB_deStegno(res);
    string ss = Bool_Str(de);
    cout << "Decrypted Message : " << ss << '\n';
    return;
}
void Jsteg_Driver() {
    string msg = "", imgAdd, tst, fileAdd;
    cout << "File to read as message : "; cin >> fileAdd;
    ifstream myfile(fileAdd);
    if (myfile.is_open()) {
        while (getline(myfile, tst))
            msg += tst;
        myfile.close();
    }
    // cout << "Message to encrypt in image : "; cin >> msg;
    vector<bool> binaryMsg = ConvertBin(msg);
    // for (bool i : binaryMsg) cout << i; cout << '\n';
    Mat img;
    cout << "Image adderess (or) image filename : "; cin >> imgAdd;
    img = imread(imgAdd, 1);
    return;
}

int main()
{
    // test();
    LSB_Driver();
    /*
        JSTEG Steganography..
    */
    Jsteg_Driver();
    return 0;
}
