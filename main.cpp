#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>

using namespace std;

const auto IMAGE_WIDTH = 400;
const auto IMAGE_HEIGHT = 300;
const auto TEXT_LEFT = 20;
const auto TEXT_BASELINE = 20;
const auto TEXT_WIDTH = 50;
const auto BIN_HEIGHT = 30;
const auto BLOCK_WIDTH = 10;

struct Input  {
vector<double> numbers;
size_t bin_count;
};

vector<double> input_numbers(ifstream& in, size_t count) {
    in >> count;
    vector<double> result(count);
    for (size_t i = 0; i < count; i++) {
       in >> result[i];
    }
    return result;
}

Input read_input(ifstream& in) {
Input data;
size_t number_count;
in >> number_count;
data.numbers = input_numbers(in, number_count);
in >> data.bin_count;
cerr << number_count << "\n";
for (int i = 0; i < number_count; i++) cerr << data.numbers[i] << " ";
cerr << "\n" << data.bin_count;

return data;
}

void find_minmax(std::vector<double> numbers, double &min, double &max) {
min = numbers[0];
max = min;
for (int i = 1; i < numbers.size(); i++) {
    if (numbers[i] < min) min = numbers[i];
    if (numbers[i] > max) max = numbers[i];
}
}

vector <size_t> make_histogram(vector<double>numbers,int bin_count) {
vector<size_t> bins(bin_count);
double min, max;
find_minmax(numbers, min, max);
double bin_size = (max - min) / bin_count;
    size_t max_count = 0;
    for (size_t i = 0; i < numbers.size(); i++) {
        bool found = false;
        for (size_t j = 0; (j < bin_count - 1) && !found; j++) {
            auto lo = min + j * bin_size;
            auto hi = min + (j + 1) * bin_size;
            if ((lo <= numbers[i]) && (numbers[i] < hi)) {
                bins[j]++;
                found = true;
                if (bins[j] > max_count) max_count = bins[j];
            }
        }
        if (!found) {
            bins[bin_count - 1]++;
            if (bins[bin_count-1] > max_count) max_count = bins[bin_count-1];
        }
    }
    return bins;
}

string svg_begin(double width, double height) {
    string svg = "<?xml version='1.0' encoding='UTF-8'?>\n";
svg.append("<svg width='");
svg.append(to_string(width));
svg.append("' ");
svg.append("height='");
svg.append(to_string(height));
svg.append("' ");
svg.append("viewBox='0 0 ");
svg.append(to_string(width));
svg.append(" ");
 svg.append(to_string(height));
svg.append("' " );
svg.append("xmlns='http://www.w3.org/2000/svg'>\n");
return svg;
}

string svg_end() {
    string svg = "</svg>\n";
    return svg;
}

string svg_rect(double x, double y, double width, double height, string stroke = "red", string fill = "#ffeeee"){
    string rect = "<rect x='";
   rect.append(to_string(x));
    rect.append("' y='");
   rect.append(to_string(y));
  rect.append("' width='");
  rect.append(to_string(width));
   rect.append( "' height='");
   rect.append( to_string(height));
   rect.append( "' stroke='");
    rect.append(stroke);
    rect.append("' fill='");
    rect.append(fill);
    rect.append("'/>");
    return rect;
}

string svg_text(double left, double baseline, string text, string decor = "none") {
    string txt =  "<text x='" ;
     txt.append(to_string(left));
     txt.append("' y='");
       txt.append(to_string(baseline));
       string mas[] = { "none", "underline", "overline", "line-through"};
       bool ok = false;
       for (int i = 0; i < 4; i++) {
                if (decor == mas[i]) {
                    ok = true;
                    break;
                }
            }
       if (!ok) {
            cout << "\n Choose text style correctly: none, underline, overline, line-through.\n";
            cin >> decor;
            for (int i = 0; i < 4; i++) {
                if (decor == mas[i]) {
                    ok = true;
                    break;
                }
            }
            if (!ok) decor = "none";
       }
        txt.append("' text-decoration= '");
        txt.append(decor);
       txt.append("'>");
        txt.append(text);
         txt.append(  "</text>");

    return txt;
}

int find_max(const vector<size_t>& bins) {
size_t max = bins[0];
for (int i = 1; i < bins.size(); i++) {
    if (bins[i] > max) max = bins[i];
}
return max;
}

void show_histogram_svg(const vector<size_t>& bins, string filePath) {
    ofstream svg_output;
    svg_output.open(filePath, ios::out | ios::trunc);
    svg_output << svg_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
    double top = 0;
    size_t max = find_max(bins);
    for (size_t bin : bins) {
        const double bin_width = (IMAGE_HEIGHT) *  static_cast<double>(bin) / max;
        svg_output << svg_text(TEXT_LEFT, top + TEXT_BASELINE, to_string(bin), "overline");
        svg_output << svg_rect(TEXT_WIDTH, top, bin_width, BIN_HEIGHT);
        top += BIN_HEIGHT;
    }
    svg_output << svg_text(TEXT_LEFT, TEXT_BASELINE, to_string(bins[0]), "overline");
    svg_output << svg_end();
    svg_output.close();
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
            CURL *curl = curl_easy_init();
if(curl) {
       CURLcode res;
  curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
       res = curl_easy_perform(curl);
       curl_easy_cleanup(curl);
}
       return 0;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    string fp;
    cout << "Enter input file path: ";
    cin >> fp;
    ifstream file;
    file.open(fp, ios::in);

    Input data = read_input(file);
    auto bins = make_histogram(data.numbers, data.bin_count);
    show_histogram_svg(bins, "1.svg");
}
