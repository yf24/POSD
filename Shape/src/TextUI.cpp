#include "TextUI.h"
#include <sstream>
#include <iostream>
#include <cstring>
#include "Builders.h"
#include <stdio.h>
#include <cstdlib>
#include <algorithm>

string help(){
    ostringstream sout;
	sout << "This system provide some instructions for user.\n"
        << "For example:\n"
        << "\'def\' name = Shape(x,y,z,...) or name = combo{name,name}\n"
        << "\'name.area\' or \'name.perimeter\'\n"
        << "\'save\' name to \"File_Name.txt\" & \'load\' \"File_Name.txt\"\n"
        << "\'add\' name to name(combo) & \'show\'\n"
        << "\'delete\' name or name from name(combo)\n";

	return sout.str();
}
void TextUI::run(){
    cout<<help();
    while(1){
        cout<< ":- ";
        string userInput;
        getline(cin,userInput);
        //cout<<input<<endl;
        analysisInstructions(userInput);
    }
}

void TextUI::analysisInstructions(string userInput){
    if(userInput == "show"){
        instructionShow();
        return;
    }

    int firstSpace = userInput.find_first_of(' ');
    if(firstSpace == string::npos) return;
    string instruction = userInput.substr(0, firstSpace);
    string content = userInput.substr(firstSpace, userInput.length()-1);

    if(instruction == "def" && content.length()){
        instructionDefine(content);
        return;
    }
    if(instruction == "delete" && content.length()){
        instructionDelete(content);
        return;
    }
    if(instruction == "add" && content.length()){
        instructionAdd(content);
        return;
    }
}

vector<string> getTokens(string content, string spliter){
    vector<string> tokens;

    char* src=new char[content.length()+1];
    strcpy(src, content.c_str());

    char* ptr=strtok(src, spliter.c_str());
    while(ptr!=NULL) {
        string piece=string(ptr);
        piece.erase(0,piece.find_first_not_of(" ")); /**trim head space*/
        piece.erase(piece.find_last_not_of(" ") + 1);/**trim last space*/
        tokens.push_back(string(piece));
        ptr=strtok(NULL, spliter.c_str());
    }

    return tokens;
}
bool isDigits(const string &str){
    return all_of(str.begin(), str.end(), ::isdigit);
}
void TextUI::instructionDefine(string content){
    //cout<<content<<endl;
    vector<string> tokens=getTokens(content, "=,() {}");

    //cout<<tokens.size()<<endl; for(string token: tokens) cout<<token<<endl;
    if(tokens.size()<2) return;

    /**build Media*/
    if(tokens[1]!="combo" && tokens.size()>3){
        /**check parameter is digit*/
        for(int i=2; i<tokens.size(); i++)
            if(!isDigits(tokens[i])) return;

        /**build ShapeMedia*/
        ShapeMediaBuilder smb;
        if(tokens[1]=="Circle"){
            /**Circle has only 3 parameters*/
            if(tokens.size()-2 == 3){
                double param1=atof(tokens[2].c_str());
                double param2=atof(tokens[3].c_str());
                double param3=atof(tokens[4].c_str());
                Circle* cir=new Circle(param1,param2,param3,tokens[0]);
                smb.buildShapeMedia(cir);
            }
            else{
                cout<<"Input illegal (Circle has only 3 parameters).\n";
                return;
            }
        }
        else if(tokens[1]=="Rectangle"){
            /**Rectangle has only 4 parameters*/
            if(tokens.size()-2 == 4){
                double param1=atof(tokens[2].c_str());
                double param2=atof(tokens[3].c_str());
                double param3=atof(tokens[4].c_str());
                double param4=atof(tokens[5].c_str());
                Rectangle* rect=new Rectangle(param1,param2,param3,param4,tokens[0]);
                smb.buildShapeMedia(rect);
            }
            else{
                cout<<"Input illegal (Rectangle has only 4 parameters).\n";
                return;
            }
        }
        else if(tokens[1]=="Triangle"){
            /**Triangle has only 6 parameters*/
            if(tokens.size()-2 == 6){
                double param1=atof(tokens[2].c_str());
                double param2=atof(tokens[3].c_str());
                double param3=atof(tokens[4].c_str());
                double param4=atof(tokens[5].c_str());
                double param5=atof(tokens[6].c_str());
                double param6=atof(tokens[7].c_str());
                Triangle* tri=new Triangle(param1,param2,param3,param4,param5,param6,tokens[0]);
                smb.buildShapeMedia(tri);
            }
            else{
                cout<<"Input illegal (Triangle has only 6 parameters).\n";
                return;
            }
        }

        /**find key*/
        if(_medias.find(tokens[0]) == _medias.end()){
            printf(">> %s (", tokens[1].c_str());
            for(int i=2; i<tokens.size(); i++){
                if(i==2) cout<<tokens[i];
                else cout<<","<<tokens[i];
            }
            cout<<")\n";
            _medias[tokens[0]] = smb.getMedia();
        }
        else printf("%s already exists.\n", tokens[0].c_str());
    }
    /**build ComboMedia*/
    else if(tokens[1]=="combo"){
        ComboMediaBuilder cmb;
        cmb.buildComboMedia();
        /**at least 2 shapes to compose*/
        if(tokens.size()-2 >= 2){
            for(int i=2; i<tokens.size(); i++){
                if(_medias.find(tokens[i]) == _medias.end()){
                    cout<<tokens[i]<<" not found.\n";
                    return;
                }
                else{
                    Media* m = _medias.find(tokens[i])->second;
                    cmb.addMedia(m);
                }
            }
        }
        else{
            cout<<"At least 2 shapes to compose.\n";
            return;
        }

        /**find key*/
        if(_medias.find(tokens[0]) == _medias.end())
            _medias[tokens[0]] = cmb.getMedia();
        else printf("%s already exists.\n", tokens[0].c_str());
    }
}

void TextUI::instructionShow() const{
    if(_medias.empty()) cout<<"There is nothing yet.\n";
    else{
        for(auto it: _medias)
            cout<<"   "<<it.first<<endl;
        /**media test*/
        for(auto it: _medias){
            Media* m = it.second;
            DescriptionVisitor dv;
            m->accept(&dv);
            cout<<dv.getDescription()<<endl;
        }
    }
}

void TextUI::instructionDelete(string content){
    vector<string> tokens=getTokens(content, "=,() {}");
    if(tokens.size()<1) return;

    if(tokens.size()==1){
        if(_medias.find(tokens[0]) == _medias.end())
            cout<<tokens[0]<<" does not exist.\n";
        else{
            auto targetIt =_medias.find(tokens[0]);
            for(auto it: _medias){
                if(ComboMedia* cm=dynamic_cast<ComboMedia*>(it.second))
                    cm->removeMedia(targetIt->second);
            }
            _medias.erase(targetIt);
        }

        return;
    }
    else if(tokens.size()==3){
        if(tokens[1]=="from"){
            auto firstMediaIt=_medias.find(tokens[0]);
            auto comboMediaIt=_medias.find(tokens[2]);

            if(firstMediaIt == _medias.end()){
                cout<<tokens[0]<<" does not exist.\n";
                return;
            }
            else if(comboMediaIt == _medias.end()){
                cout<<tokens[2]<<" does not exist.\n";
                return;
            }
            else{
                if(ComboMedia* c=dynamic_cast<ComboMedia*>(comboMediaIt->second))
                    ((ComboMedia*)comboMediaIt->second)->removeMedia(firstMediaIt->second);
                else{
                    cout<<"Should input a ComboMedia after \"from\".\n";
                    return;
                }
            }
        }
    }
}

void TextUI::instructionAdd(string content){
    vector<string> tokens=getTokens(content, "=,() {}");
    if(tokens.size()<1) return;

    if(tokens.size()==3){
        if(tokens[1]=="to"){
            auto firstMediaIt=_medias.find(tokens[0]);
            auto comboMediaIt=_medias.find(tokens[2]);

            if(firstMediaIt == _medias.end()){
                cout<<tokens[0]<<" does not exist.\n";
                return;
            }
            else if(comboMediaIt == _medias.end()){
                cout<<tokens[2]<<" does not exist.\n";
                return;
            }
            else if(tokens[0]==tokens[2]){
                cout<<"Can not add itself.\n";
                return;
            }
            else{
                if(ComboMedia* c=dynamic_cast<ComboMedia*>(comboMediaIt->second))
                    ((ComboMedia*)comboMediaIt->second)->add(firstMediaIt->second);
                else{
                    cout<<"Should input a ComboMedia after \"to\".\n";
                    return;
                }
            }
        }
    }
}
