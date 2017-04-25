/****************************************************************************
  FileName  [ floorplanner.cpp ]
  Synopsis  [ Implementation of the floorplanner based on b*-tree. ]
  Author    [ Fu-Yu Chuang ]
  Date      [ 2017.4.25 ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include "floorplanner.h"
using namespace std;

double Floorplanner::getHPWL() const
{
    double HPWL = 0;
    for (size_t i = 0, end = _netList.size(); i < end; ++i) {
        HPWL += _netList[i]->calcHPWL();
    }
    return (HPWL / 2.0);
}


void Floorplanner::readCircuit(fstream& inBlk, fstream& inNet)
{
    this->readBlock(inBlk);
    this->readNet(inNet);

    return;
}

void Floorplanner::floorplan()
{

    return;
}

bool Floorplanner::checkFit()
{
    return ((Block::getMaxX() <= _width) && (Block::getMaxY() <= _height));
}

void Floorplanner::printSummary() const
{
    double wireLength = this->getHPWL();
    double area = this->getArea();
    cout << endl;
    cout << "==================== Summary ====================" << endl;
    cout << " Cost: "   << _alpha * area + (1 - _alpha) * wireLength << endl;
    cout << " Wire: "   << wireLength << endl;
    cout << " Area: "   << area << endl;
    cout << " Width: "  << Block::getMaxX() << endl;
    cout << " Height: " << Block::getMaxY() << endl;
    cout << "=================================================" << endl;
    return;
}

void Floorplanner::reportBlock() const
{
    // TODO: report position
    assert(_blockNum == _blockList.size());
    cout << "Number of blocks: " << _blockNum << endl;
    for (size_t i = 0, end = _blockList.size(); i < end; ++i) {
        cout << setw(8) << _blockList[i]->getName() << setw(6)
             << _blockList[i]->getWidth() << setw(6) << _blockList[i]->getHeight();
        cout << endl;
    }
    return;
}

void Floorplanner::reportTerm() const
{
    assert(_termNum == _termList.size());
    cout << "Number of terminals: " << _termNum << endl;
    for (size_t i = 0, end = _termList.size(); i < end; ++i) {
        cout << setw(8) << _termList[i]->getName() << setw(6)
             << _termList[i]->getX1() << setw(6) << _termList[i]->getY1();
        cout << endl;
    }

    return;
}

void Floorplanner::reportNet() const
{
    assert(_netNum == _netList.size());
    cout << "Number of nets: " << _netNum << endl;
    for (size_t i = 0, end_i = _netList.size(); i < end_i; ++i) {
        const vector<Terminal*> termList = _netList[i]->getTermList();
        for (size_t j = 0, end_j = termList.size(); j < end_j; ++j) {
            cout << setw(6) << termList[j]->getName();
        }
        cout << endl;
    }

    return;
}

void Floorplanner::writeResult(fstream& outFile)
{
    stringstream buff;
    double wireLength = this->getHPWL();
    double area = this->getArea();

    // <final cost>
    buff << (_alpha * area + (1 - _alpha) * wireLength);
    outFile << buff.str() << '\n';
    buff.str("");

    // <total wirelength>
    buff << wireLength;
    outFile << buff.str() << '\n';
    buff.str("");

    // <chip_area>
    buff << area;
    outFile << buff.str() << '\n';
    buff.str("");

    // <chip_width> <chip_height>
    buff << Block::getMaxX();
    outFile << buff.str() << " ";
    buff.str("");
    buff << Block::getMaxY();
    outFile << buff.str() << '\n';
    buff.str("");

    // <program_runtime>
    buff << (double)(_stop - _start) / CLOCKS_PER_SEC;
    outFile << buff.str() << '\n';
    buff.str("");

    // <macro_name> <x1> <y1> <x2> <y2>
    for (size_t i = 0, end = _blockList.size(); i < end; ++i) {
        outFile << _blockList[i]->getName() << " "
                << _blockList[i]->getX1() << " " << _blockList[i]->getY1() << " "
                << _blockList[i]->getX2() << " " << _blockList[i]->getY2() << '\n';
    }

    return;
}

// private member functions
void Floorplanner::readBlock(fstream& inBlk)
{
    string str;

    // Outline: <outline width, outline height>
    inBlk >> str;
    assert(str == "Outline:");
    inBlk >> str;
    _width = stoi(str);
    inBlk >> str;
    _height = stoi(str);

    // NumBlocks: <# of blocks>
    inBlk >> str;
    assert(str == "NumBlocks:");
    inBlk >> str;
    _blockNum = stoi(str);

    // NumTerminals: <# of terminals>
    inBlk >> str;
    assert(str == "NumTerminals:");
    inBlk >> str;
    _termNum = stoi(str);

    // read macros
    // <macro name> <macro width> <macro height>
    for (size_t i = 0; i < _blockNum; ++i) {
        string name;
        size_t width, height;
        inBlk >> name;
        inBlk >> str;
        width = stoi(str);
        inBlk >> str;
        height = stoi(str);
        _blockList.push_back(new Block(name, width, height));
        _termName2Ptr[name] = _blockList.back();
    }

    // read terminals
    // <terminal name> terminal <x coordinate> <y coordinate>
    for (size_t i = 0; i < _termNum; ++i) {
        string name;
        size_t x, y;
        inBlk >> name;
        inBlk >> str;
        inBlk >> str;
        x = stoi(str);
        inBlk >> str;
        y = stoi(str);
        _termList.push_back(new Terminal(name, x, y));
        _termName2Ptr[name] = _termList.back();
    }

    return;
}

void Floorplanner::readNet(fstream& inNet)
{
    string str;

    // NumNets: <# of nets>
    inNet >> str;
    assert(str == "NumNets:");
    inNet >> str;
    _netNum = stoi(str);

    // read nets
    // NetDegree: <# of terminals in this net>
    // <terminal name> ...
    for (size_t i = 0; i < _netNum; ++i) {
        size_t termNum;
        inNet >> str;
        assert(str == "NetDegree:");
        inNet >> str;
        termNum = stoi(str);
        _netList.push_back(new Net());
        for (size_t j = 0; j < termNum; ++j) {
            inNet >> str;
            _netList.back()->addTerm(_termName2Ptr[str]);
        }
    }

    return;
}