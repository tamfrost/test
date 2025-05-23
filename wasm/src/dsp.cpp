#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <map>
#include "SignalAnalyser.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(MyLib) {
//        register_vector<double>("vector<double>");
//        register_map<std::string, std::vector<double>>("map<string, vector<double>>");

        class_<SignalAnalyser>("SignalAnalyser")
            .constructor<int,double,double,float,float,float,float,float>()
            .function("shiftFrequency", &SignalAnalyser::shiftFrequency)
            .function("demodulate", &SignalAnalyser::demodulate)
            .function("ifft", &SignalAnalyser::ifft)
            .function("fft", &SignalAnalyser::fft)
            .class_function("getCompilationTime", &SignalAnalyser::getCompilationTime)
            .class_function("getCommitHash", &SignalAnalyser::getCommitHash)
        ;

        enum_<ModulationType>("ModulationType")
        .value("AM", ModulationType::AM)
        .value("LSB", ModulationType::LSB)
        .value("USB", ModulationType::USB)
        .value("FM", ModulationType::FM)
        .value("CW", ModulationType::FM)
        .value("NONE", ModulationType::NONE)
        ;

//        value_object<DfAnalyzer::BearingsReturnStruct>("ReturnStruct")
//        .field("nPoints", &DfAnalyzer::BearingsReturnStruct::nPoints)
//        .field("nBearings", &DfAnalyzer::BearingsReturnStruct::nBearings)
//        .field("mainLinePtr", &DfAnalyzer::BearingsReturnStruct::mainLinePtr)
//        .field("mainLinePtr", &DfAnalyzer::BearingsReturnStruct::mainLinePtr)
//        .field("upperLinePtr", &DfAnalyzer::BearingsReturnStruct::upperLinePtr)
//        .field("lowerLinePtr", &DfAnalyzer::BearingsReturnStruct::lowerLinePtr)
//        .field("mainMeanLinePtr", &DfAnalyzer::BearingsReturnStruct::mainMeanLinePtr)
//        .field("upperMeanLinePtr", &DfAnalyzer::BearingsReturnStruct::upperMeanLinePtr)
//        .field("lowerMeanLinePtr", &DfAnalyzer::BearingsReturnStruct::lowerMeanLinePtr)
//        .field("idPtr", &DfAnalyzer::BearingsReturnStruct::idPtr)
//        .field("includePtr", &DfAnalyzer::BearingsReturnStruct::includePtr)
//        .field("noIncludedBearings", &DfAnalyzer::BearingsReturnStruct::noIncludedBearings)
//        ;
//
//        value_object<DfAnalyzer::CrossesReturnStruct>("ReturnStruct")
//        .field("nCrosses", &DfAnalyzer::CrossesReturnStruct::nCrosses)
//        .field("coordinatePtr", &DfAnalyzer::CrossesReturnStruct::coordinatePtr)
//        ;
//
//        value_object<DfAnalyzer::FixEllipseReturnStruct>("ReturnStruct")
//        .field("centerLat", &DfAnalyzer::FixEllipseReturnStruct::centerLat)
//        .field("centerLon", &DfAnalyzer::FixEllipseReturnStruct::centerLon)
//        .field("majorAxes", &DfAnalyzer::FixEllipseReturnStruct::majorAxes)
//        .field("minorAxes", &DfAnalyzer::FixEllipseReturnStruct::minorAxes)
//        .field("angle", &DfAnalyzer::FixEllipseReturnStruct::angle)
//        ;

}