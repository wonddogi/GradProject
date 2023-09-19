#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <utility>
using namespace std;


// Tae Young's code start
int t_size;
vector<char> alphabet;
unordered_map<string, pair<vector<int>, vector<int> > > precomputed_values;

int countdone;

void debugTable(const vector<vector<int> > table) {
  for (auto v : table) {
    for (auto i : v)
      cout << i << " ";
    cout << endl;
  }
}

void debugVector(const vector<int> vec) {
  for (auto i : vec)
    cout << i << " ";
  cout << endl;
}

pair<vector<int>, vector<int> > PrecomputeSingle(
    const string &row_string, // length t - 1 string
    const string &column_string, // length t - 1 string
    const vector<int> &row_offset_vector, // length t - 1 offset vector
    const vector<int> &column_offset_vector) { // length t - 1 offset vector
  // table initialization
  vector<vector<int> > table(t_size, vector<int>(t_size));
  table[0][0] = 0;
  for (int i = 1; i < t_size; i++) {
    table[0][i] = row_offset_vector[i - 1] + table[0][i - 1];
    table[i][0] = column_offset_vector[i - 1] + table[i - 1][0];
  }

  // table calculation
  for (int row = 1; row < t_size; row++) {
    for (int col = 1; col < t_size; col++) {
      int t = (row_string[col - 1] == column_string[row - 1]) ? 0 : 1;
      int diagonal = table[row - 1][col - 1] + t;
      int vertical = table[row - 1][col] + 1;
      int horizontal = table[row][col - 1] + 1;
      table[row][col] = min(diagonal, min(vertical, horizontal));
    }
  }

  // returning the result row and column offset vectors
  // t_size -> t_size -1 로 수정함
  vector<int> row_offset_vector_output(t_size-1);
  vector<int> column_offset_vector_output(t_size-1);

  for (int i = 1; i < t_size; i++) {
    row_offset_vector_output[i-1] = table[t_size - 1][i] - table[t_size - 1][i - 1];
    column_offset_vector_output[i-1] = table[i][t_size - 1] - table[i - 1][t_size - 1];
  }
  countdone++;

  string key = row_string + column_string;
  for (auto v: row_offset_vector) key.append(to_string(v));
  for (auto v: column_offset_vector) key.append(to_string(v));
  precomputed_values.insert(make_pair(key, make_pair(row_offset_vector_output, column_offset_vector_output)));

  // debug
  /*
  cout << key << " ";
  for (auto v: row_offset_vector_output) cout << to_string(v);
  cout << " ";
  for (auto v: column_offset_vector_output) cout << to_string(v);
  cout << endl;
  */
  return make_pair(row_offset_vector_output, column_offset_vector_output);
}

void PossibleOffsets(const string &rowstr, const string &colstr,
                    vector<int> row, vector<int> col,
                    int kr, int kc) {
  if (kr == 0 && kc == 0) {
    PrecomputeSingle(rowstr, colstr, row, col);
  } else if (kr == 0) {
    for (auto i : {-1, 0, 1}) {
      col.push_back(i);
      PossibleOffsets(rowstr, colstr, row, col, kr, kc - 1);
      col.pop_back();
    }
  } else {
    for (auto i : {-1, 0, 1}) {
      row.push_back(i);
      PossibleOffsets(rowstr, colstr, row, col, kr - 1, kc);
      row.pop_back();
    }
  }
}

void PossibleStringsOffsets(string rowstr, string colstr,
                            int kr, int kc) {
  if (kr == 0 && kc == 0) {
    PossibleOffsets(rowstr, colstr, {}, {}, t_size - 1, t_size - 1);
  } else if (kr == 0) {
    for (auto c : alphabet) PossibleStringsOffsets(rowstr, colstr + c, kr, kc - 1);
  } else {
    for (auto c : alphabet) PossibleStringsOffsets(rowstr + c, colstr, kr - 1, kc);
  }
}
// Tae Young's code end


// Jae Hyun's code start
int compute_t_block_in_k(int k,int t, string P, string T) {
    int n = P.length(); // abcdefgh 0~7 length = 8
    int m = T.length();

    // (i,i) 와 (i,i-k-(t-1)) 사이의 t-block 개수
    int k_t = (k-1)/(t-1)+1;
    
    vector<int> row(t-1,+1); // first row의 offset은 모두 +1
    vector<vector<int> > row_list(2*k_t + 2, vector<int>(t-1,+1)); // 필요한 row vector 개수: 2*k_t+2

    for(int i=0; i<min(k_t+1,n/(t-1)); i++) { // row의 왼쪽 k+t 경계보다 0이 더 큰 경우
        string sub_p = P.substr(i*(t-1),t-1);
        vector<int> col(t-1,+1); // 각 row의 첫번째 t-block의 column offset은 모두 +1
    	
        // row list[i] 사용한 compute 결과 row list[i]에 업데이트
        for(int j=0; j<min(i+k_t+1,m/(t-1)); j++) {
            string sub_t = T.substr(j*(t-1),t-1);
            
            // precomputing 결과 가져오기
            string key = sub_t + sub_p;
            for(auto v: row_list[j]) key.append(to_string(v)); 
            for(auto v: col) key.append(to_string(v));
	    
	    if(precomputed_values.find(key) == precomputed_values.end()) return -1;
	    auto value = precomputed_values.find(key)->second;

            // value 값을 row 값과 col 값으로 나누기............
            vector<int> row_value = get<0>(value);
            vector<int> col_value = get<1>(value);

            // precomputing 결과 col, row_list에 저장
            row_list[j] = row_value;
            col = col_value;
        }
    }
    
    int distance = min(k_t+1,n/(t-1))*(t-1);
    for(auto v: row_list[0]) distance += v;

    if(k_t+1 >= n/(t-1)) {
	    // 마지막 t-block의 마지막 셀 값의 edit_distance
	    cout << "row_list: ";
	    for(int i=1; i<m/(t-1)-n/(t-1)+min(k_t+1,n/(t-1)); i++) {
		    for(auto v: row_list[i]) {
			    distance += v;
			    cout << v << " ";
		    }
	    }
	    cout << endl;
	    return distance;
    }

    for(int i=k_t+1; i<n/(t-1); i++) { // row의 왼쪽 k+t 경계가 0보다 큰 경우
        string sub_p = P.substr(i*(t-1),t-1);
        vector<int> col(t-1,+1); // 각 row의 첫번째 t-block의 column offset은 모두 +1
       
	// row list[x] 사용한 compute 결과 row list[x-1]에 업데이트 (x은 1부터 계산)
        for(int j=i-k_t; j<min(i+k_t+1,m/(t-1)); j++) {
            string sub_t = T.substr(j*(t-1),t-1);
            
            // precomputing 결과 가져오기
            string key = sub_t + sub_p;
            for(auto v: row_list[j-i+k_t+1]) key.append(to_string(v));
            for(auto v: col) key.append(to_string(v));
            
	    if(precomputed_values.find(key) == precomputed_values.end()) return -1;
	    auto value = precomputed_values.find(key)->second;

            // value 값을 row 값과 col 값으로 나누기............
            vector<int> row_value = get<0>(value);
            vector<int> col_value = get<1>(value);

            // precomputing 결과 col, row_list에 저장
            row_list[j-i+k_t] = row_value;
            col = col_value;
        }
        
        // 각 row의 첫번째 t-block의 첫번째 셀 값 업데이트
        distance += (t-1);
        for(auto v: row_list[0]) distance += v;
    }

    // 마지막 t-block의 마지막 셀 값의 edit_distance
    for(int i=1; i<m/(t-1)-n/(t-1)+min(k_t+1,n/(t-1)); i++) {
        for(auto v: row_list[i]) distance += v;
    }

    // n,m 이 t-block에 맞는 값으로 설정되있는 경우만 제대로 작동
    // 밑의 나머지 값들을 계산하는 코드 추가 필요............
    return distance;
}

int main(void) {
    int alphabet_size;
    int k_size;
    string pattern;
    string text;

    cout << "alphabet set size: ";
    cin >> alphabet_size;

    cout << "alphabets: ";
    for (int i = 0; i < alphabet_size; ++i) {
        char c;
        cin >> c;
        alphabet.push_back(c);
    }

    cout << "t-block size: ";
    cin >> t_size;

    cout << endl;
    cout << "Please wait until precomputing is done....." << endl;
    PossibleStringsOffsets("", "", t_size - 1, t_size - 1);
    cout << "precomputing is done!!" << endl;

    int edit_distance;
    while(1) {
	    cout << "Enter pattern or 'q' for exit: ";
	    cin >> pattern;
	    if(pattern == "q" || pattern == "Q") break;
	    cout << "Enter text or 'q' for exit: ";
	    cin >> text;
	    if(text == "q" || text == "Q") break;
	    cout << "Enter k size: ";
	    cin >> k_size;

	    int diff = text.length() - pattern.length();
	    while(k_size < abs(diff)) {
		    cout << endl;
		    cout << "k is smaller than m-n!! Re-enter k size: ";
		    cin >> k_size;
	    }
	    edit_distance = compute_t_block_in_k(k_size,t_size,pattern, text);

	    if(edit_distance == -1) {
		    cout << "Text or Pattern contain unacceptable alphabets" << endl;
		    continue;
	    }
	    cout << endl;
	    cout << "pattern: " << pattern << endl;
	    cout << "text: " << text << endl;
	    cout << "distance: " << edit_distance << endl;
	    cout << endl;
    }
    return 0;
}

/*
// padding
void padding(int t, string& P, string& T) {
    int n = P.length();
    int m = T.length();

    int front_pad, back_pad;
    int rem_n = n % (t-1);
    int rem_m = m % (t-1);

    char alphabet_pad = P[0];   // pattern 의 첫번째 문자로 padding

    front_pad = min(rem_n,rem_m);
    back_pad = abs(rem_n - rem_m);

    P.insert(0, front_pad, alphabet_pad);
    P.insert(P.length(), back_pad, alphabet_pad);
    T.insert(0, front_pad, alphabet_pad);
    T.insert(T.length(), back_pad, alphabet_pad);
}
*/
