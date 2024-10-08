/*****************************************************************************
 *
 *   Command line and input parameter parsing + simulation file output
 *
 ****************************************************************************/
/*
 *   This file is part of GPUMCML.
 *
 *   GPUMCML is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   GPUMCML is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with GPUMCML.  If not, see <http://www.gnu.org/licenses/>.
 */

#define NFLOATS 5
#define NINTS 5

#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "../tqdm/tqdm.h"
#include "CLI11.h"
#include "gpumcml.h"

using namespace std;

struct CommandLineArguments g_commandLineArguments;

//////////////////////////////////////////////////////////////////////////////
//   Parse command line arguments
//////////////////////////////////////////////////////////////////////////////
int interpret_arg(int argc, char *argv[])
{
    CLI::App app{"Monte Carlo Multi-Layer (MCML) accelerated by GPU"};
    // add version callback
    app.add_flag_callback(
        "--version,-V",
        [&] {
            std::cout << "Version: " << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "."
                      << PROJECT_VERSION_PATCH << "\n"
                      << "Build details:"
                      << "\n"
                      << "\tCommit SHA: " << GIT_COMMIT << "\n"
                      << "\tSystem: " << CMAKE_SYSTEM << "\n"
                      << "\tProcessor: " << CMAKE_SYSTEM_PROCESSOR << "\n"
                      << "\tCompiler: " << CMAKE_CXX_COMPILER << "\n"
                      << "\tDate: " << BUILD_TIMESTAMP << "\n";
            exit(0);
        },
        "Print the version information");
    // add options to CLI
    auto input_file = app.add_option("-i,--input", g_commandLineArguments.input_file,
                                     "Path to the .mci file that contains the tissue configuration.");
    input_file->required();
    auto output_file = app.add_option("-O,--output", g_commandLineArguments.output_file,
                                      "Path to file where the output will be stored. Make sure that the parent folder "
                                      "already exists. The file name will be created on the parent folder.");
    output_file->required();
    app.add_option("-S,--seed", g_commandLineArguments.seed, "Seed.");
    app.add_option("-G,--n_gpus", g_commandLineArguments.number_of_gpus, "Number of GPUs to use.");
    app.add_flag("-A,--ignore_absorption", g_commandLineArguments.ignore_absorption_detection,
                 "Indicates that absorption detection should not be recorded. It can speed up simulations in some "
                 "cases, but will not be able to calculate penetration depth.");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::CallForHelp &e)
    {
        app.exit(e);
        exit(0);
    }
    catch (const CLI::ParseError &e)
    {
        return app.exit(e);
    }
    return 0;
}

/***********************************************************
 *	Write the input parameters to the file.
 ****/
void WriteInParm(FILE *file, SimulationStruct *sim)
{
    unsigned int i;

    fprintf(file, "InParm \t\t\t# Input parameters. cm is used.\n");

    fprintf(file, "%s \tA\t\t# output file name, ASCII.\n", sim->outp_filename);
    fprintf(file, "%u \t\t\t# No. of photons\n", sim->number_of_photons);

    fprintf(file, "%G\t%G\t\t# dz, dr [cm]\n", sim->det.dz, sim->det.dr);
    fprintf(file, "%u\t%u\t%u\t# No. of dz, dr, da.\n\n", sim->det.nz, sim->det.nr, sim->det.na);

    fprintf(file, "%u\t\t\t\t\t# Number of layers\n", sim->n_layers);

    fprintf(file, "#n\tmua\tmus\tg\td\t# One line for each layer\n");
    fprintf(file, "%G\t\t\t\t\t# n for medium above\n", sim->layers[0].n);

    for (i = 1; i <= sim->n_layers; i++)
    {
        fprintf(file, "%G\t%G\t%G\t%G\t%G\t# layer %hd\n", sim->layers[i].n, sim->layers[i].mua,
                1 / sim->layers[i].mutr - sim->layers[i].mua, sim->layers[i].g,
                sim->layers[i].z_max - sim->layers[i].z_min, i);
    }
    fprintf(file, "%G\t\t\t\t\t# n for medium below\n", sim->layers[i].n);
}

int isnumeric(char a)
{
    if (a >= (char)48 && a <= (char)57)
        return 1;
    else
        return 0;
}

int readfloats(int n_floats, float *temp, FILE *pFile)
{
    int ii = 0;
    char mystring[200];

    if (n_floats > NFLOATS)
        return 0; // cannot read more than NFLOATS floats

    while (ii <= 0)
    {
        if (feof(pFile))
            return 0; // if we reach EOF here something is wrong with the file!
        char *result = fgets(mystring, 200, pFile);
        if (result == NULL)
        {
            perror("Error white reading stream from file");
        }
        memset(temp, 0, NFLOATS * sizeof(float));
        ii = sscanf(mystring, "%f %f %f %f %f", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
        if (ii > n_floats)
            return 0;
    }
    return 1; // Everyting appears to be ok!
}

int readints(int n_ints, int *temp, FILE *pFile) // replace with template?
{
    int ii = 0;
    char mystring[STR_LEN];

    if (n_ints > NINTS)
        return 0; // cannot read more than NFLOATS floats

    while (ii <= 0)
    {
        if (feof(pFile))
            return 0; // if we reach EOF here something is wrong with the file!
        char *result = fgets(mystring, STR_LEN, pFile);
        if (result == NULL)
        {
            perror("Error white reading stream from file");
        }
        memset(temp, 0, NINTS * sizeof(int));
        ii = sscanf(mystring, "%d %d %d %d %d", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
        if (ii > n_ints)
            return 0;
    }
    return 1; // Everyting appears to be ok!
}

int ischar(char a)
{
    if ((a >= (char)65 && a <= (char)90) || (a >= (char)97 && a <= (char)122))
        return 1;
    else
        return 0;
}

//////////////////////////////////////////////////////////////////////////////
//   Parse simulation input file
//////////////////////////////////////////////////////////////////////////////
int read_simulation_data(const char *filename, SimulationStruct **simulations, int ignoreAdetection)
{
    int i = 0;
    int ii = 0;
    unsigned long number_of_photons;
    int n_simulations = 0;
    int n_layers = 0;
    FILE *pFile;
    char mystring[STR_LEN];
    char str[STR_LEN];
    char AorB;
    float dtot = 0;

    float ftemp[NFLOATS]; // Find a more elegant way to do this...
    int itemp[NINTS];

    double n1, n2, r;

    pFile = fopen(filename, "r");
    if (pFile == NULL)
    {
        perror("Error opening file");
        return 0;
    }

    // First read the first data line (file version) and ignore
    if (!readfloats(1, ftemp, pFile))
    {
        perror("Error reading file version");
        return 0;
    }

    // Second, read the number of runs
    if (!readints(1, itemp, pFile))
    {
        perror("Error reading number of runs");
        return 0;
    }
    n_simulations = itemp[0];

    // Allocate memory for the SimulationStruct array
    *simulations = (SimulationStruct *)malloc(sizeof(SimulationStruct) * n_simulations);
    if (*simulations == NULL)
    {
        perror("Failed to malloc simulations.\n");
        return 0;
    }

    tqdm pbar;
    for (i = 0; i < n_simulations; i++)
    {
        if (i == 0)
        {
            printf("Reading Simulations MCI file\n");
        }
        // Store the input filename
        strcpy((*simulations)[i].inp_filename, filename);

        // Store ignoreAdetection data
        (*simulations)[i].ignoreAdetection = ignoreAdetection;

        // Read the output filename and determine ASCII or Binary output
        ii = 0;
        while (ii <= 0)
        {
            (*simulations)[i].begin = ftell(pFile);
            char *result = fgets(mystring, STR_LEN, pFile);
            if (result == NULL)
            {
                perror("Error white reading stream from file");
            }
            ii = sscanf(mystring, "%s %c", str, &AorB);
            if (feof(pFile) || ii > 2)
            {
                perror("Error reading output filename");
                return 0;
            }
            if (ii > 0)
                ii = ischar(str[0]);
        }
        strcpy((*simulations)[i].outp_filename, str);
        (*simulations)[i].AorB = AorB;

        // Read the number of photons
        ii = 0;
        while (ii <= 0)
        {
            char *result = fgets(mystring, STR_LEN, pFile);
            if (result == NULL)
            {
                perror("Error white reading stream from file");
            }
            number_of_photons = 0;
            ii = sscanf(mystring, "%lu", &number_of_photons);
            if (feof(pFile) || ii > 1)
            {
                perror("Error reading number of photons");
                return 0;
            }
        }
        (*simulations)[i].number_of_photons = number_of_photons;

        // Read dr and dz (2x float)
        if (!readfloats(2, ftemp, pFile))
        {
            perror("Error reading dr and dz");
            return 0;
        }
        (*simulations)[i].det.dz = ftemp[0];
        (*simulations)[i].det.dr = ftemp[1];

        // Read No. of dz, dr and da  (3x int)
        if (!readints(3, itemp, pFile))
        {
            perror("Error reading No. of dz, dr and da");
            return 0;
        }
        (*simulations)[i].det.nz = itemp[0];
        (*simulations)[i].det.nr = itemp[1];
        (*simulations)[i].det.na = itemp[2];

        // Read No. of layers (1xint)
        if (!readints(1, itemp, pFile))
        {
            perror("Error reading No. of layers");
            return 0;
        }
        n_layers = itemp[0];
        (*simulations)[i].n_layers = itemp[0];

        // Allocate memory for the layers (including one for the upper and one for the lower)
        (*simulations)[i].layers = (LayerStruct *)malloc(sizeof(LayerStruct) * (n_layers + 2));
        if ((*simulations)[i].layers == NULL)
        {
            perror("Failed to malloc layers.\n");
            return 0;
        }

        // Read upper refractive index (1xfloat)
        if (!readfloats(1, ftemp, pFile))
        {
            perror("Error reading upper refractive index");
            return 0;
        }
        (*simulations)[i].layers[0].n = ftemp[0];

        dtot = 0;
        for (ii = 1; ii <= n_layers; ii++)
        {
            // Read Layer data (5x float)
            if (!readfloats(5, ftemp, pFile))
            {
                perror("Error reading layer data");
                return 0;
            }
            (*simulations)[i].layers[ii].n = ftemp[0];
            (*simulations)[i].layers[ii].mua = ftemp[1];
            (*simulations)[i].layers[ii].g = ftemp[3];
            (*simulations)[i].layers[ii].z_min = dtot;
            dtot += ftemp[4];
            (*simulations)[i].layers[ii].z_max = dtot;
            if (ftemp[2] == 0.0f)
                (*simulations)[i].layers[ii].mutr = FLT_MAX; // Glas layer
            else
                (*simulations)[i].layers[ii].mutr = 1.0f / (ftemp[1] + ftemp[2]);
        } // end ii<n_layers

        // Read lower refractive index (1xfloat)
        if (!readfloats(1, ftemp, pFile))
        {
            perror("Error reading lower refractive index");
            return 0;
        }
        (*simulations)[i].layers[n_layers + 1].n = ftemp[0];

        (*simulations)[i].end = ftell(pFile);

        // calculate start_weight
        n1 = (*simulations)[i].layers[0].n;
        n2 = (*simulations)[i].layers[1].n;
        r = (n1 - n2) / (n1 + n2);
        r = r * r;
        (*simulations)[i].start_weight = 1.0F - (float)r;

        pbar.progress(i, n_simulations);

    } // end for i<n_simulations
    printf("\n");
    return n_simulations;
}

void FreeSimulationStruct(SimulationStruct *sim, int n_simulations)
{
    int i;
    for (i = 0; i < n_simulations; i++)
        free(sim[i].layers);
    free(sim);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SimulationResults::registerSimulationResults(SimState *HostMem, SimulationStruct *sim)
{
    int na = sim->det.na; // Number of grid elements in angular-direction [-]
    int nr = sim->det.nr; // Number of grid elements in r-direction
    int nz = sim->det.nz; // Number of grid elements in z-direction
    float dz = sim->det.dz;
    float tissueDept = 0;

    int rz_size = nr * nz;
    int ra_size = nr * na;
    int i;

    double scale1 = (double)(WEIGHT_SCALE) * (double)sim->number_of_photons;

    // Calculate and write RAT
    UINT64 Rd = 0; // Diffuse reflectance [-]
    UINT64 A = 0;  // Absorbed fraction [-]
    UINT64 T = 0;  // Transmittance [-]
    float penetrationDepth = 0;
    UINT64 beamIntensityAtPenetrationDepth;
    UINT64 weightPenetration = 0;

    for (i = 0; i < rz_size; i++)
    {
        A += HostMem->A_rz[i];
    }
    for (i = 0; i < ra_size; i++)
    {
        T += HostMem->Tt_ra[i];
        Rd += HostMem->Rd_ra[i];
    }

    // get tissue depth, it can be less than dimensions of grid. Layer number 0 has depth 0, only used for specular
    // reflections
    tissueDept = sim->layers[sim->n_layers].z_max;

    // get penetration depth, ignore Rd because penetration depth should be estimated based on the intensity of the beam
    // just below the surface of the tissue (no reflection).
    beamIntensityAtPenetrationDepth = (A + T) * (1. / EULER);
    for (int iz = 0; iz < nz; ++iz)
    {
        for (int ir = 0; ir < nr; ++ir)
        {
            unsigned index = ir * nz + iz;
            // A_rz stores values in column-major order, this means that all absorption values A_rz[0 ... nz] correspond
            // to the first element of the radial direction. Here the index is recomputed to sum all radial values
            // for each z value first.
            weightPenetration += HostMem->A_rz[index];
            if (weightPenetration > beamIntensityAtPenetrationDepth)
            {
                penetrationDepth = static_cast<float>(iz) * dz; // gets penetration depth in cm
                break;
            }
            else if ((index == rz_size - 1) && (weightPenetration > 0.))
            {
                // when the beam has gone through the whole layered tissue but there is a lot of transmission such that
                // the intensity of the beam does not reduce to I/e within the tissue, the penetration depth has to be
                // the whole thickness of the tissue model
                penetrationDepth = tissueDept;
                break;
            }
        }
    }
    this->resultsStream << sim->outp_filename << "," << 1.0F - sim->start_weight << "," << (double)Rd / scale1 << ",";
    this->resultsStream << (double)A / scale1 << "," << (double)T / scale1 << "," << (double)penetrationDepth << "\n";
}

void SimulationResults::writeSimulationResults(const char *mcoFile)
{
    FILE *pFile_outp;
    pFile_outp = fopen(mcoFile, "a");
    if (pFile_outp == NULL)
    {
        perror("Error opening output file");
        cout << "unable to open mcoFile";
    }
    std::string results = this->resultsStream.str();
    fprintf(pFile_outp, "%s", results.c_str());
    fclose(pFile_outp);
}
