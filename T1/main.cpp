#include <iostream>
#include <string>
#include <fstream>
//#include <vector>
//#include <array>
#include <stack>
//#include <sstream>
#include <list>
#include <map>
//#include <vector>
#include <queue>
//#include <typeinfo>
  //! gets filename from input stream
std::string get_file_name();
  //! check if the file is open and stores each char of the file 
std::list<char> create_char_array_of_file(std::fstream& tfile);
  //! gets one of the attributes from every image and pushes it onto the list
void get_atribute(std::string line, std::string info, std::list<std::string>& atribute, std::string::size_type j);
  //! algorithm
int labelling(std::string name, std::string height, std::string width, std::string data);
int main() {
    auto file_name = get_file_name();
    
    std::fstream file(file_name);
    
    std::stack<std::string> stack;
    std::list<std::string> name;
    std::list<std::string> height;
    std::list<std::string> width;
    std::list<std::string> data;
    
    bool read = false;
    bool data_read = false;
    bool ident = true;
    int m = 0;
    std::string temp_data = "";
    if (file.is_open()) {
        std::string line = "";
        while(std::getline(file, line)) {
            if (line.find("data") == line.npos) {
                temp_data = temp_data + line;
            }
            // for para percorrer a string da linha obtida com getline
            for (std::string::size_type i = 0; i < line.size(); ++i) {
                if (line[i] == '<') {
                    auto desempilha = false;
                    if(line[i+1] == '/') {
                        i++;
                        desempilha = true;
                    }
                    std::string info ="";
                    for(std::string::size_type j = i+1; j < line.size(); ++j) {
                        if (line[j] == '>' && desempilha) {  ///< se encontrou o fechamento e desempilhar for vdd
                            if (!stack.empty()) {
                                if (stack.top().compare(info) == 0) {  ///< checa se o alvo a desempilhar é o topo
                                    //std::cout << "desempilhando: " << info << std::endl;
                                    stack.pop();  ///< se for o topo desempilha
                                    data_read = false;
                                    break;
                                } else {  ///< se não, lança erro
                                    //throw std::out_of_range("tentou desempilhar não o topo");
                                    std::cout << "error"<< std::endl;
                                    ident = false;
                                }
                            } else {
                                std::cout << "error"<< std::endl;
                                ident = false;
                            }
                        } else if (line[j] == '>' && !desempilha) {  ///< se encontrou fechamento e desempilhar for falso
                                //std::cout << "empilhando: " << info << std::endl;
                                stack.push(info);  ///< empilha o tag
                                if(info.compare("name") == 0)
                                    get_atribute(line, info, name, j);
                                if(info.compare("height") == 0)
                                    get_atribute(line, info, height, j);
                                if(info.compare("width") == 0)
                                    get_atribute(line, info, width, j);
                                
                                break;
                            }
                            if (line.find("/data") != line.npos) {  // se encontrar /data
                                    data.push_back(temp_data);
                                    temp_data = "";
                            }
                        info = info + (line[j]);
                    }
                }
            }
        }
    } else {
            printf("didnt open!\n");
        }

    if (!stack.empty()) {
        // throw std::out_of_range("pilha nao vazia");
        std::cout << "error"<< std::endl;
    }
    





    for (auto it: data) {
        data.remove("");
    }
    //std::cout << data.size();
///////////////////////////////////////////// segunda parte
    if (ident) {
        auto size = name.size();
        for (int i = 0; i != size; i++) {
            std::cout << name.front() << " " << labelling(name.front(), height.front(), width.front(), data.front()) << std::endl;
            name.pop_front();height.pop_front();width.pop_front();data.pop_front();
        }
    }

    return 0;
}

std::string get_file_name() {
    std::string xmlfilename;
    std::cin >> xmlfilename;  // entrada
    return xmlfilename;
}

void get_atribute(std::string line, std::string info, std::list<std::string>& atribute, std::string::size_type j) {

        std::string name_ = "";
        for(std::string::size_type k = j+1; k != -1; ++k)
        {
            if (line[k] == '<') {
                break;
            }
            name_ = name_ +""+ (line[k]);
        }
        atribute.push_back(name_);
}
int labelling(std::string name, std::string height, std::string width, std::string data) {
    auto _height = stoi(height);
    auto _width = stoi(width);
    //std::cout << name << " rola " << _height << " penis " << _width << "caralho" << data;
    char copy[_height][_width];
    char reference[_height][_width];
    //std::pair(int, int) copy;
    //std::pair(int, int) original;
    
     for (int i = 0; i != _height; i++)
         for (int j = 0; j != _width; j++) {
            copy[i][j] = '0'; // de baixo pra cima 
         }
    // for (int i = 0; i != _width; i++) {
    //      for (int j = 0; j != _height; j++) {
    //         std::cout << copy[j][i] <<" "; // de baixo pra cima 
    //      }
    //         std::cout << "\n";
    // }

    int j = 0;
    for (int i = 0; i != data.size(); i++) {
        
        if (i%_width == 0 && i != 0) {
            j++;
        }
        /////////////////////////////////////////////ESQUERDA PRA DIREITA
        reference[j][i%_width] = data.at(i); 
        //std::cout<< "is: " << reference[j][i%_width] << " expected: " << data.at(i) <<std::endl;
    }
    //std::cout << "\n";
    char label = '1';
    std::queue<std::pair<int, int>> kiwi;
    for (int i = 0; i != _height; i++) {
        for (int k = 0; k != _width; k++) {
            //std::cout << reference[i][k]; // <<" i: " << i << " j: " << j << std::endl; de baixo pra cima 
            if (reference[i][k] == '1' && copy[i][k] == '0') {
                kiwi.push(std::make_pair(i, k));
                while(!kiwi.empty()){
                    auto temp = kiwi.front();
                    kiwi.pop();
                    //std::cout << "tipo " << typeid(label).name() << " label: " << label << " ";
                    copy[temp.first][temp.second] = label; // pondo label na copia de zeros
                    if (temp.first-1 >= 0)
                        if(reference[temp.first-1][temp.second] == '1' && copy[temp.first-1][temp.second] == '0')
                            kiwi.push(std::make_pair(temp.first-1, temp.second));
                    if (temp.first+1 < _height)
                        if(reference[temp.first+1][temp.second] == '1' && copy[temp.first+1][temp.second] == '0')
                            kiwi.push(std::make_pair(temp.first+1, temp.second));
                    if (temp.second-1 >= 0)
                        if (reference[temp.first][temp.second-1] == '1' && copy[temp.first][temp.second-1] == '0')
                            kiwi.push(std::make_pair(temp.first, temp.second-1));
                    if (temp.second+1 < _width)
                        if (reference[temp.first][temp.second+1] == '1' && copy[temp.first][temp.second+1] == '0')
                            kiwi.push(std::make_pair(temp.first, temp.second+1));
                }
                int num = (int)label - 48;
                //std::cout << "num:" << num << std::endl;
                num++;
                label = static_cast<char>(num + 48);
            }
        }
            //std::cout << "\n";
    }

    
    return (int)label - 49;;
}
