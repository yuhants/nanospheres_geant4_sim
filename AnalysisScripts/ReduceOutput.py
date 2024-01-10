import sys, os
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import scipy
import pickle 
import uproot

## ----------------------------- ##
##     USER DEFINED INPUTS       ##
## ----------------------------- ##

## Analyze 1 file at a time for now
filePath = '../Geant4_CCD_simulation/output/Ion_Cr51_0keV_1umSphere_1cmMaxStep_SpherePos.root'

## save file?
saveFile = True

## saved file name
outputFilePath = '../Data/Cr51_decay_Exits_1cmMaxStep_1umNanosphere_SpherePos_data.pkl'

## ----------------------------- ##
##    CONVERT FROM BRANCH TO     ##
##       ARRAY OF ARRAYS         ##
## ----------------------------- ##

def BranchToVector(tree, branchName, STLtype=False, vecType=False):
    
    branchOptions = list(tree.keys())
    if branchName not in branchOptions:
        print('Branch <',branchName,'> not in branchOptions')
        return None

    arr=None
    if STLtype:
        arrIn = np.array(tree[branchName])
        if vecType:
            arrOut = []
            [arrOut.append(np.array(list(arrIn)[i])) for i in range(len(arrIn))]
        else:
            arrOut = np.array(list(arrIn))
    else:
        arrOut = np.array(tree[branchName])

    return arrOut

## ----------------------------- ##
##          OPEN FILE            ##
## ----------------------------- ##

## Use uproot to open the File
file = uproot.open(filePath)

## Only one tree in each file
tree = file['Events']

## view branches in tree
print('Available branches are: \n', tree.keys())

## ------------------- ##
## GET USEFUL BRANCHES ##
## ------------------- ##
print('Reading Selected Branches...')
## Note that string vectors are handled differently

## Primary Info
primary_pNames = BranchToVector(tree, 'primaries.particleName', STLtype=True)
#primary_PIDs = BranchToVector(tree, 'primaries.pid')
primary_energykeV = BranchToVector(tree, 'primaries.energy_keV')
primary_momentumkeV = BranchToVector(tree, 'primaries.totalMomentum_keV')
primary_x_mm = BranchToVector(tree, 'primaries.position_X_mm')
primary_y_mm = BranchToVector(tree, 'primaries.position_Y_mm')
primary_z_mm = BranchToVector(tree, 'primaries.position_Z_mm')
primary_dirX = BranchToVector(tree, 'primaries.direction_X')
primary_dirY = BranchToVector(tree, 'primaries.direction_Y')
primary_dirZ = BranchToVector(tree, 'primaries.direction_Z')

## nanosphere Exit Info
exit_trackIDS = BranchToVector(tree, 'exits.trackID')
exit_pnames = BranchToVector(tree, 'exits.particleName', STLtype=True, vecType=True)
exit_postStepEnergyKeV = BranchToVector(tree, 'exits.postStepEnergy_keV')
exit_particleCharge = BranchToVector(tree, 'exits.particleCharge')
exit_particleMass = BranchToVector(tree, 'exits.particleMass_keV')
exit_postStepPositionX_um = BranchToVector(tree, 'exits.postStepPositionX_um')
exit_postStepPositionY_um = BranchToVector(tree, 'exits.postStepPositionY_um')
exit_postStepPositionZ_um = BranchToVector(tree, 'exits.postStepPositionZ_um')
exit_postStepMomentumXkeV = BranchToVector(tree, 'exits.postStepMomentumX_keV')
exit_postStepMomentumYkeV = BranchToVector(tree, 'exits.postStepMomentumY_keV')
exit_postStepMomentumZkeV = BranchToVector(tree, 'exits.postStepMomentumZ_keV')

## Full Track info
track_IDs = BranchToVector(tree, 'tracks.trackID')
track_stepNums = BranchToVector(tree, 'tracks.stepNumber')
track_parentID = BranchToVector(tree, 'tracks.parentID')
track_detName = BranchToVector(tree, 'tracks.detectorName', STLtype=True, vecType=True)
#track_detID = BranchToVector(tree, 'tracks.detectorID')
track_pName = BranchToVector(tree, 'tracks.particleName', STLtype=True, vecType=True)
track_creationProc = BranchToVector(tree, 'tracks.creationProcess', STLtype=True, vecType=True)
track_particleMasskeV = BranchToVector(tree,'tracks.particleMass_keV')
track_preStepEnergykeV = BranchToVector(tree, 'tracks.preStepEnergy_keV')
track_EnergyDepkeV = BranchToVector(tree, 'tracks.energyDeposition_keV')
track_preStepMomentumXkeV = BranchToVector(tree, 'tracks.preStepMomentumX_keV')
track_preStepMomentumYkeV = BranchToVector(tree, 'tracks.preStepMomentumY_keV')
track_preStepMomentumZkeV = BranchToVector(tree, 'tracks.preStepMomentumZ_keV')
track_postStepMomentumXkeV = BranchToVector(tree, 'tracks.postStepMomentumX_keV')
track_postStepMomentumYkeV = BranchToVector(tree, 'tracks.postStepMomentumY_keV')
track_postStepMomentumZkeV = BranchToVector(tree, 'tracks.postStepMomentumZ_keV')
track_implantDepth_um = BranchToVector(tree, 'tracks.implantDepth_um')
track_stepTime_ns = BranchToVector(tree, 'tracks.stepTime_ns')
track_x_global_um = BranchToVector(tree, 'tracks.x_global_um')
track_y_global_um = BranchToVector(tree, 'tracks.y_global_um')
track_z_global_um = BranchToVector(tree, 'tracks.z_global_um')
#track_x_local_um = BranchToVector(tree, 'tracks.x_local_um')
#track_y_local_um = BranchToVector(tree, 'tracks.y_local_um')
#track_z_local_um = BranchToVector(tree, 'tracks.z_local_um')

## ----------------------------------------------- ##
##                 EVENT PROCESSING                ##
## ----------------------------------------------- ##

## This could be the length of any of the above vectors
nEvents = len(primary_pNames)
print(nEvents, "Events in file!")
print('Starting to analyze events...')

## Primary particle info per event
primaryX_mm, primaryY_mm, primaryZ_mm = np.zeros(nEvents), np.zeros(nEvents), np.zeros(nEvents)
primaryDirX, primaryDirY, primaryDirZ = np.zeros(nEvents), np.zeros(nEvents), np.zeros(nEvents)
primaryParticleEnergy = np.zeros(nEvents)
primaryParticleMomentum = np.zeros(nEvents)

## total event Momentum 
totalEventMomentum = np.zeros(nEvents)

## Si CCD hit multiplicity per event
numSiPanelsHit = np.zeros(nEvents)

## Max penetration depth in each event
maxPeneDepths = {'Si1':np.zeros(nEvents), 'Si2':np.zeros(nEvents), 'Si3':np.zeros(nEvents), 
                    'Si4':np.zeros(nEvents), 'Si5':np.zeros(nEvents), 'Si6':np.zeros(nEvents)}

## Total Edep in all volumes
totEdepAllVol = {"nanosphere":np.zeros(nEvents), 'Si1':np.zeros(nEvents), 'Si2':np.zeros(nEvents), 
                 'Si3':np.zeros(nEvents), 'Si4':np.zeros(nEvents), 'Si5':np.zeros(nEvents), 'Si6':np.zeros(nEvents)}

## Total Edep in CCDs pasth some dead-layer
totEdepCCDVol_20nm = {'Si1':np.zeros(nEvents), 'Si2':np.zeros(nEvents), 'Si3':np.zeros(nEvents), 
                    'Si4':np.zeros(nEvents), 'Si5':np.zeros(nEvents), 'Si6':np.zeros(nEvents)}
totEdepCCDVol_100nm = {'Si1':np.zeros(nEvents), 'Si2':np.zeros(nEvents), 'Si3':np.zeros(nEvents), 
                    'Si4':np.zeros(nEvents), 'Si5':np.zeros(nEvents), 'Si6':np.zeros(nEvents)}

## Total momentum imparted to nanosphere
totMomentumNanosphere = np.zeros((nEvents, 3))
totMomentumCCD_20nm = {'Si1':np.zeros((nEvents, 3)), 'Si2':np.zeros((nEvents, 3)), 'Si3':np.zeros((nEvents, 3)), 
                       'Si4':np.zeros((nEvents, 3)), 'Si5':np.zeros((nEvents, 3)), 'Si6':np.zeros((nEvents, 3))}
totMomentumCCD_100nm = {'Si1':np.zeros((nEvents, 3)), 'Si2':np.zeros((nEvents, 3)), 'Si3':np.zeros((nEvents, 3)), 
                        'Si4':np.zeros((nEvents, 3)), 'Si5':np.zeros((nEvents, 3)), 'Si6':np.zeros((nEvents, 3))}

## Store particles depositing energy/momentum in Nanosphere
EdepNanosphereParticles = {'V51':np.zeros(nEvents), 'gamma':np.zeros(nEvents), 
                           'e-':np.zeros(nEvents), 'nu_e':np.zeros(nEvents)}
MOdepNanosphereParticles = {'V51':np.zeros((nEvents, 3)), 'gamma':np.zeros((nEvents, 3)), 
                            'e-':np.zeros((nEvents, 3)), 'nu_e':np.zeros((nEvents, 3))}

## Store interesting events
tracks_steps_hit2, tracks_steps_hit3, tracks_steps_hit4 = [], [], []

## visualizing the detector 
detPositions_mm = {"nanosphere":[], 'Si1':[], 'Si2':[], 'Si3':[], 'Si4':[], 'Si5':[], 'Si6':[]}

## store all radioactive decay daughters
radDecayProg = []

## store steps of particles not leaving nanosphere
nanosphereParticles = []

## store all exiting particles
nansphereExitParticles = []

## Exit Energy Per Particle
exitEnergyParticles = {'V51':np.zeros(nEvents), 'gamma':np.zeros(nEvents), 'e-':np.zeros(nEvents), 'nu_e':np.zeros(nEvents)}

exitMomentumParticles = {'V51':np.zeros((nEvents, 3)), 'gamma':np.zeros((nEvents, 3)), 'e-':np.zeros((nEvents, 3)), 'nu_e':np.zeros((nEvents, 3))}

exitParticleCharge = np.zeros(nEvents)
exitParticleMultiplicity = np.zeros(nEvents)

## ------------------------------ ##
##    Loop through each event     ##
## ------------------------------ ##
for idx in range(nEvents):

    ## At this points primaries has length corresponding 
    ## to the number of primary particles generated - generally 1
    
    ## Store primary particle energy (most useful for Particle and Spectrum Generators)  
    primaryParticleEnergy[idx] = primary_energykeV[idx][0]
    ## Store first primary position and direction
    primaryX_mm[idx] = primary_x_mm[idx][0]
    primaryY_mm[idx] = primary_y_mm[idx][0]
    primaryZ_mm[idx] = primary_z_mm[idx][0]
    primaryDirX[idx] = primary_dirX[idx][0]
    primaryDirY[idx] = primary_dirY[idx][0]
    primaryDirZ[idx] = primary_dirZ[idx][0]

    ## Keep track of child's created e.g. from radioactive decays
    childMap = dict()
    exitMap = dict()
    nanosphereMap = dict()
    trackMap = dict()

    ## dummy list for volumes hit in an event
    volList = []
    tracks_steps = []
    process_list = []


    ## ------------------------------ ##
    ##      LOOP THROUGH EXITING      ##
    ##       PARTICLES IN EVENT       ##
    ## ------------------------------ ##
    if (exit_trackIDS is not None) and (len(exit_trackIDS[idx]) != 0.):

        for kdx in range(len(exit_trackIDS[idx])):

            eID = exit_trackIDS[idx][kdx]
            eParticleName = exit_pnames[idx][kdx]
            eParticleCharge = exit_particleCharge[idx][kdx]
            ePostStepEnergykeV = exit_postStepEnergyKeV[idx][kdx]
            ePostStepPositionX_um = exit_postStepPositionX_um[idx][kdx]
            ePostStepPositionY_um = exit_postStepPositionY_um[idx][kdx]
            ePostStepPositionZ_um = exit_postStepPositionZ_um[idx][kdx]
            ePostStepMomentumXkeV = exit_postStepMomentumXkeV[idx][kdx]
            ePostStepMomentumYkeV = exit_postStepMomentumYkeV[idx][kdx]
            ePostStepMomentumZkeV = exit_postStepMomentumZkeV[idx][kdx]

            ## Record Energy of particles leaving nanosphere
            if eParticleName in list(exitEnergyParticles.keys()):
                exitEnergyParticles[eParticleName][idx] += ePostStepEnergykeV

            if (eID not in exitMap):
                exitMap[eID] = [eParticleName, eParticleCharge, ePostStepEnergykeV,
                                [ePostStepPositionX_um, ePostStepPositionY_um, ePostStepPositionZ_um],
                                [ePostStepMomentumXkeV, ePostStepMomentumYkeV, ePostStepMomentumZkeV]
                                ]

            ## Record momentum of particles leaving nanosphere
            if eParticleName in list(exitMomentumParticles.keys()):
                exitMomentumParticles[eParticleName][idx][0] += ePostStepMomentumXkeV
                exitMomentumParticles[eParticleName][idx][1] += ePostStepMomentumYkeV
                exitMomentumParticles[eParticleName][idx][2] += ePostStepMomentumZkeV

            ## tabulate total charge per event
            exitParticleCharge[idx] += eParticleCharge

            ## cound particle multiplicity
            exitParticleMultiplicity[idx] += 1

    ## ------------------------------ ##
    ##        LOOP THROUGH ALL        ##
    ##        TRACKS IN EVENT         ##
    ## ------------------------------ ##
    totMomentumX_keV, totMomentumY_keV, totMomentumZ_keV = 0., 0., 0.
    tID_previous = -1
    ## Note that trackIDs are unique
    if (track_IDs is not None) and (len(track_IDs[idx]) != 0.):
        for jdx in range(len(track_IDs[idx])):

            ## define the characteristics of a given step
            tID = track_IDs[idx][jdx]
            tStepNum = track_stepNums[idx][jdx]
            tParentID = track_parentID[idx][jdx]
            tDetName = track_detName[idx][jdx]
            tParticleName = track_pName[idx][jdx]
            tCreationProc = track_creationProc[idx][jdx]
            tParticleMasskeV = track_particleMasskeV[idx][jdx]
            tPreStepEnergykeV = track_preStepEnergykeV[idx][jdx]
            tEnergyDepkeV = track_EnergyDepkeV[idx][jdx]
            tPreStepMomentumXkeV = track_preStepMomentumXkeV[idx][jdx]
            tPreStepMomentumYkeV = track_preStepMomentumYkeV[idx][jdx]
            tPreStepMomentumZkeV = track_preStepMomentumZkeV[idx][jdx]
            tPostStepMomentumXkeV = track_postStepMomentumXkeV[idx][jdx]
            tPostStepMomentumYkeV = track_postStepMomentumYkeV[idx][jdx]
            tPostStepMomentumZkeV = track_postStepMomentumZkeV[idx][jdx]
            tImplantDepth_um = track_implantDepth_um[idx][jdx]
            t_time_ns = track_stepTime_ns[idx][jdx]
            t_glob_x_um = track_x_global_um[idx][jdx]
            t_glob_y_um = track_y_global_um[idx][jdx]
            t_glob_z_um = track_z_global_um[idx][jdx]

            ## useful for printing events
            if tID != tID_previous:
                #print(tID, tParentID, tParticleName, tCreationProc, tPreStepEnergykeV)
                #print(np.sqrt(tPreStepMomentumXkeV**2 + tPreStepMomentumYkeV**2 + tPreStepMomentumZkeV**2))
                #print(tPreStepMomentumXkeV, tPreStepMomentumYkeV, tPreStepMomentumZkeV)
                tID_previous = tID 

            ## total pre step momentum
            totalPreStepMomentum = np.sqrt(tPreStepMomentumXkeV**2 + tPreStepMomentumYkeV**2 + tPreStepMomentumZkeV**2)
            ## delta momentum for particles
            delta_MomementumX_keV = tPostStepMomentumXkeV - tPreStepMomentumXkeV
            delta_MomementumY_keV = tPostStepMomentumYkeV - tPreStepMomentumYkeV
            delta_MomementumZ_keV = tPostStepMomentumZkeV - tPreStepMomentumZkeV

            ## Store initial primaries of radioactive decays
            if (tID not in childMap) and (tParentID == 1 or tParentID == 0) and (tCreationProc == "RadioactiveDecay"):
                childMap[tID] = [tParticleName, tPreStepEnergykeV, [tPreStepMomentumXkeV, tPreStepMomentumYkeV, tPreStepMomentumZkeV]]
                totMomentumX_keV += tPreStepMomentumXkeV
                totMomentumY_keV += tPreStepMomentumYkeV
                totMomentumZ_keV += tPreStepMomentumZkeV

            ## store position and particle name of tracks in nanosphere
            if (tID not in nanosphereMap) and (tDetName == "nanosphere"):
                nanosphereMap[tID] = [tParticleName, [t_glob_x_um, t_glob_y_um, t_glob_z_um], [0., 0., 0.]]
            elif (tID in nanosphereMap) and (tDetName == "nanosphere"):
                nanosphereMap[tID][2] = [t_glob_x_um, t_glob_y_um, t_glob_z_um]

            ## Store track info
            if (tID not in trackMap):
                trackMap[tID] = []
                trackMap[tID].append([t_time_ns, t_glob_x_um/1e3,t_glob_y_um/1e3,t_glob_z_um/1e3])
            else:
                trackMap[tID].append([t_time_ns, t_glob_x_um/1e3,t_glob_y_um/1e3,t_glob_z_um/1e3])
            
            ## store all tracks
            #tracks_steps.append(np.array([t_time_ns, t_glob_x_um/1e3,t_glob_y_um/1e3,t_glob_z_um/1e3]))

            ## Store detector positions
            if tDetName in list(detPositions_mm.keys()):
                detPositions_mm[tDetName].append(np.array([t_glob_x_um/1e3,t_glob_y_um/1e3,t_glob_z_um/1e3]))

            ## For mono-E or spectrum beta analysis
            ## See how many volumes are hit (ignoring nanosphere)
            if tDetName not in volList and tDetName != 'nanosphere':
                volList.append(tDetName)

            ## Store energy deposited in detVol
            if tDetName in list(totEdepAllVol.keys()):
                totEdepAllVol[tDetName][idx] += tEnergyDepkeV

            ## store max penetration depth
            if (tDetName != 'nanosphere') and (tDetName in list(maxPeneDepths.keys())):
                if tImplantDepth_um > maxPeneDepths[tDetName][idx]:
                    maxPeneDepths[tDetName][idx] = tImplantDepth_um
            else:
                ## record all momenta imparted to nanosphere
                totMomentumNanosphere[idx][0] -= delta_MomementumX_keV
                totMomentumNanosphere[idx][1] -= delta_MomementumY_keV
                totMomentumNanosphere[idx][2] -= delta_MomementumZ_keV
                if tParticleName in list(EdepNanosphereParticles.keys()):
                    EdepNanosphereParticles[tParticleName][idx] += tEnergyDepkeV
                if tParticleName in list(MOdepNanosphereParticles.keys()):
                    MOdepNanosphereParticles[tParticleName][idx][0] -= delta_MomementumX_keV
                    MOdepNanosphereParticles[tParticleName][idx][1] -= delta_MomementumY_keV
                    MOdepNanosphereParticles[tParticleName][idx][2] -= delta_MomementumZ_keV

            ## Store total energy deposited beyond deadlayer (20 nm)
            if tDetName in list(totEdepCCDVol_20nm.keys()) and tImplantDepth_um > 0.02:
                totEdepCCDVol_20nm[tDetName][idx] += tEnergyDepkeV
                ## store momentum info
                totMomentumCCD_20nm[tDetName][idx][0] -= delta_MomementumX_keV
                totMomentumCCD_20nm[tDetName][idx][1] -= delta_MomementumY_keV
                totMomentumCCD_20nm[tDetName][idx][2] -= delta_MomementumZ_keV

            ## Store total energy deposited beyond deadlayer (100 nm)
            if tDetName in list(totEdepCCDVol_100nm.keys()) and tImplantDepth_um > 0.1:
                totEdepCCDVol_100nm[tDetName][idx] += tEnergyDepkeV
                ## store momentum info
                totMomentumCCD_100nm[tDetName][idx][0] -= delta_MomementumX_keV
                totMomentumCCD_100nm[tDetName][idx][1] -= delta_MomementumY_keV
                totMomentumCCD_100nm[tDetName][idx][2] -= delta_MomementumZ_keV

        ## End Loop over all tracks in Event

    ## calculate total event momentum
    totalEventMomentum = np.sqrt(totMomentumX_keV**2 + totMomentumY_keV**2 + totMomentumZ_keV**2)

    ## Store how many CCDs are hit
    numSiPanelsHit[idx] = len(volList)

    ## store step info
    if len(volList) == 2 and len(tracks_steps_hit2) == 0:
        tracks_steps_hit2.append(trackMap)
    if len(volList) == 3 and len(tracks_steps_hit3) == 0:
        tracks_steps_hit3.append(trackMap)
    if len(volList) == 4 and len(tracks_steps_hit4) == 0:
        tracks_steps_hit4.append(trackMap)

    ## Store the progeny info from this event
    radDecayProg.append(childMap)

    ## Store dict of particles in nanosphere
    nanosphereParticles.append(nanosphereMap)

    ## store dict of particles exiting
    nansphereExitParticles.append(exitMap)

## ----------------------------------------- ##
##             SAVE OUT ARRAYS               ##
##       INTO A DICT WHICH CAN BE SAVED      ##  
## ----------------------------------------- ##
print('Saving reduced arrays to file:', outputFilePath)

dict_array  = {'primaryX_mm':primaryX_mm, 'primaryY_mm':primaryY_mm, 'primaryZ_mm':primaryZ_mm, 
               'primaryDirX':primaryDirX, 'primaryDirY':primaryDirY, 'primaryDirZ':primaryDirZ,
               'primaryParticleEnergy':primaryParticleEnergy, 'DecayProgeny':radDecayProg, 'numSiPanelsHit':numSiPanelsHit,
               'tracks_steps_hit2ccds':tracks_steps_hit2, 'tracks_steps_hit3ccds':tracks_steps_hit3, 'tracks_steps_hit4ccds':tracks_steps_hit4,
               'maxPeneDepths':maxPeneDepths, 'detPositions_mm':detPositions_mm, 'detPositions_mm':detPositions_mm,
               'totEdepAllVol':totEdepAllVol, 'totEdepCCDVol_20nm':totEdepCCDVol_20nm, 'totEdepCCDVol_100nm':totEdepCCDVol_100nm,
               'totMomentumNanosphere':totMomentumNanosphere, 'totMomentumCCD_20nm':totMomentumCCD_20nm, 'totMomentumCCD_100nm':totMomentumCCD_100nm,
               'EdepNanosphereParticles':EdepNanosphereParticles, 'MOdepNanosphereParticles':MOdepNanosphereParticles,
               'nansphereExitParticles':nansphereExitParticles,
               'exitEnergyParticles':exitEnergyParticles, 'exitMomentumParticles':exitMomentumParticles, 
               'exitParticleCharge':exitParticleCharge, 'exitParticleMultiplicity':exitParticleMultiplicity,
               'nanosphereParticles':nanosphereParticles}

if saveFile:
    with open(outputFilePath, 'wb') as outFile:
        pickle.dump(dict_array, outFile)

## Last line of code
print('DONE!')