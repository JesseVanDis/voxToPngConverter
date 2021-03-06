#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <zconf.h>
#endif

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "scene.hpp"
#include "arguments.hpp"

static void printHelp(const Arguments& args)
{
	printf("Usage: voxpng [OPTIONS]\n");
	printf("converts .vox to png file(s)\n");
	#ifdef _WIN32
		printf("example: voxpng -i C:/guy.vox -o C:/guy.png\n");
	#else		
		printf("example: voxpng -i ~/guy.vox -o ~/guy.png\n");
	#endif
	printf("\n");
	args.printOptions();
	printf("\n");
	printf("if any error occurs, the exit status is 2.\n");
}

int main(int argc, char **argv)
{
	bool byDragAndDrop = false;
	Arguments args(argc, argv);	
	if(argc == 2)
	{
		char outputFilePath[512] = "";
		strcpy(outputFilePath, argv[1]);
		size_t len = strlen(outputFilePath);
		if(len > 3)
		{
			outputFilePath[len-3] = 'p';
			outputFilePath[len-2] = 'n';
			outputFilePath[len-1] = 'g';
		}
		args.clear();
		args.addArgument("i", argv[1]);
		args.addArgument("o", outputFilePath);
		byDragAndDrop = true;
	}
	
	#ifdef _WIN32
		if(argc == 1) 
		{
			printf("\n");
			printf(" =======  .VOX to .PNG converter 5000! ======= \n");
			printf("Usage: Drag & Drop the .vox file in the .exe file\n");
			printf("or call this from the command line with the '-h' option for more options\n");
			printf("Application will close in 5 seconds\n");
			printf("\n");
			SLEEP(5000);
		}
	#endif

	if(!byDragAndDrop && (argc < 3 || args.getArgument_ShouldShowHelp()))
	{
		printHelp(args);
		return 0;
	}
	else
	{
		auto voxPath = args.getArgument_InputPath();
		auto outputPath = args.getArgument_OutputPath();
		auto setting = args.getArgument_Setting();
		if(voxPath.handleError() || outputPath.handleError() || setting.handleError())
		{
			if(byDragAndDrop)
			{
				SLEEP(5000);
			}
			return 2;
		}
		
		Scene scene(args.getArgument_Verbose());
		error err = scene.load(voxPath.result);
		if(!err.empty())
		{
			printf("%s\n", err.c_str());
			if(byDragAndDrop)
			{
				SLEEP(5000);
			}
			return 2;
		}

		SavingContext savingContext;
		vector<int> clippingEdges = args.getArgument_ClipEdges();
		if(clippingEdges.size() == 6)
		{
			savingContext.clipping.left = clippingEdges[0];
			savingContext.clipping.back = clippingEdges[1];
			savingContext.clipping.bottom = clippingEdges[2];
			savingContext.clipping.right = clippingEdges[3];
			savingContext.clipping.front = clippingEdges[4];
			savingContext.clipping.top = clippingEdges[5];
		}

		vector<int> chunkDetails = args.getArgument_ChunkSizeAndPos();
		if(chunkDetails.size() >= 3)
		{
			savingContext.chunkSetup.sizeX = (uint)max(chunkDetails[0], 0);
			savingContext.chunkSetup.sizeY = (uint)max(chunkDetails[1], 0);
			savingContext.chunkSetup.sizeZ = (uint)max(chunkDetails[2], 0);
			if(chunkDetails.size() == 6)
			{
				savingContext.chunkSetup.offsetX = chunkDetails[3];
				savingContext.chunkSetup.offsetY = chunkDetails[4];
				savingContext.chunkSetup.offsetZ = chunkDetails[5];
			}
		}

		savingContext.pBorderColor = args.getArgument_BorderColor();
		savingContext.removeHiddenVoxels = args.getArgument_RemoveHiddenVoxels();

		switch(setting.result)
		{
			case Setting_SeperatePngs:
				err = scene.saveAsPngArray(outputPath.result, savingContext);
				break;

			case Setting_Merged:
				err = scene.saveAsMergedPng(outputPath.result, savingContext);
				break;
		}
		if(!err.empty())
		{
			printf("%s\n", err.c_str());
			if(byDragAndDrop)
			{
				SLEEP(5000);
			}
			return 2;
		}

		if(args.getArgument_Verbose())
		{
			printf("done");
		}
		return 0;
	}
}