#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include "LitGCode.h"
#include "Traitement.h"
#include "params.h"

using namespace std;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc,char **argv)
{
    string GCodeFile;
    string confFile;
    Params params;
    /*
     * Options uniquement pour la ligne de commande
     */
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "print version string")
        ("help", "produce help message")    
        ("config,c",po::value<string>(&confFile),"configuration file")
        ;

    /*
     * Options pour la ligne de commande et le fichier de paramètres
     */
    po::options_description config("Allowed options");
    config.add_options()
        ("stretch",po::value<int>(&params.stretch)->default_value(170),"Décalage en microns")
        ("width",po::value<int>(&params.wallWidth)->default_value(700),"Largeur de piste en microns")
        ("nozzle",po::value<int>(&params.nozzleDiameter)->default_value(800),"Diamètre du bec en microns")
        ("dumpLayer",po::value<int>(&params.dumpLayer)->default_value(0),"Générer une image pour une couche")
        ;

    /*
     * Options cachées
     */
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input-file", po::value<string>(&GCodeFile)->default_value(""), "g-code file name")
        ;


    po::positional_options_description p;
    p.add("input-file", -1);

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);


    try
    {
        po::variables_map vm;
        //po::store(po::parse_command_line(argc, argv, desc), vm);
        po::store(po::command_line_parser(argc, argv).
                options(cmdline_options).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << visible << "\n";
            return 0;
        }
        if (vm.count("version"))
        {
            cout << "post_stretch version 1.0" << endl;
            return 0;
        }
        if (!confFile.empty())
        {
            ifstream isp(confFile.c_str());
            if (!isp.is_open())
            {
                cerr << "Unable to open configuration file " << confFile << endl;
                return -1;
            }
            po::store(po::parse_config_file(isp,config_file_options),vm);
            po::notify(vm);
        }
        /*
        if (rbFile.empty())
        {
            cerr << "You must specify input file name" << endl;
            cerr << visible << endl;
            return -1;
        }
        */
        unique_ptr<Traitement> traitement(FabriqueTraitement(params));
        LitGCode(traitement.get());
    }
    catch (std::exception& err)
    {
        cerr << err.what() << endl;
        return -1;
    }
    return 0;
}

