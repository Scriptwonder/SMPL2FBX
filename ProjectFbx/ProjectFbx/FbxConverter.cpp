#include <fbxsdk.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string>

using namespace std;
class FBXController {
public:
    std::string csvFilePath = "my_file_newlong2.csv";
    std::string fbxFilePath = "male.fbx";
    std::string fbxFilePath1 = "male2.fbx";
    std::vector<std::string> joints = { "m_avg_Pelvis","m_avg_L_Hip","m_avg_R_Hip","m_avg_Spine1",
    "m_avg_L_Knee","m_avg_R_Knee","m_avg_Spine2","m_avg_L_Ankle","m_avg_R_Ankle","m_avg_Spine3"
    ,"m_avg_L_Foot","m_avg_R_Foot","m_avg_Neck"
    ,"m_avg_L_Collar","m_avg_R_Collar"
    ,"m_avg_Head","m_avg_L_Shoulder","m_avg_R_Shoulder"
    ,"m_avg_L_Elbow","m_avg_R_Elbow","m_avg_L_Wrist","m_avg_R_Wrist","m_avg_L_Hand","m_avg_R_Hand" };

    int totalFrame;
    int jointNumber = 25;
    
    vector<vector<float>> npyArray;
    vector<vector<float>> translationArray;

    void Start() {
        FbxManager* fbxManager = FbxManager::Create();
        
        // similar to the Unity version, you would:
        // 1. prepare your NPY data
        npyArray = prepareNpy();
        // 2. create the importer, initialize it, set the IO settings, import the scene
        FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);

        fbxManager->SetIOSettings(fbxIOSettings);
        FbxScene* fbxScene = FbxScene::Create(fbxManager, "myScene");

        FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "Importer");
        bool status = fbxImporter->Initialize(fbxFilePath.c_str(), -1, fbxIOSettings);

        if (status) {
           fbxIOSettings->SetBoolProp(EXP_FBX_MATERIAL, true);
           fbxIOSettings->SetBoolProp(EXP_FBX_TEXTURE, true);
           fbxIOSettings->SetBoolProp(EXP_FBX_EMBEDDED, true);
           fbxIOSettings->SetBoolProp(EXP_FBX_SHAPE, true);
           fbxIOSettings->SetBoolProp(EXP_FBX_GOBO, true);
           fbxIOSettings->SetBoolProp(EXP_FBX_ANIMATION, true);
           fbxIOSettings->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
        }

        status = fbxImporter->Import(fbxScene);
        if (!status) {
			std::cout << "Error: " << fbxImporter->GetStatus().GetErrorString() << std::endl;
		}

        // 3. set the scene info
        FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(fbxManager, "SceneInfo");
        sceneInfo->mTitle = "myScene";
        sceneInfo->mSubject = "mySceneAKA";
        sceneInfo->mAuthor = "Shutong";
        sceneInfo->mRevision = "1.0";
        sceneInfo->mKeywords = "myScene";
        sceneInfo->mComment = "myScene";

        fbxScene->SetSceneInfo(sceneInfo);
        // 4. add animation
        addAnimation(fbxScene, fbxManager);

        // 5. save the scene
        saveScene(fbxManager, fbxScene, fbxFilePath1);
        // 6. clean up resources
        // don't forget to destroy the manager at the end
        fbxImporter->Destroy();
        fbxManager->Destroy();
    }

    void saveScene(FbxManager* fbxManager, FbxScene* fbxScene, std::string write_path) {
        // similar to the Unity version...
        FbxExporter* fbxExporter = FbxExporter::Create(fbxManager, "Exporter");
        FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
        fbxManager->SetIOSettings(fbxIOSettings);

        int fileFormat = fbxManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)");
        fbxIOSettings->SetBoolProp(EXP_FBX_MATERIAL, true);
        fbxIOSettings->SetBoolProp(EXP_FBX_TEXTURE, true);
        fbxIOSettings->SetBoolProp(EXP_FBX_EMBEDDED, true);
        fbxIOSettings->SetBoolProp(EXP_FBX_SHAPE, true);
        fbxIOSettings->SetBoolProp(EXP_FBX_GOBO, true);
        fbxIOSettings->SetBoolProp(EXP_FBX_ANIMATION, true);
        fbxIOSettings->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);
        bool status = fbxExporter->Initialize(write_path.c_str(), fileFormat, fbxManager->GetIOSettings());
        if (!status) {
			std::cout << "Error: " << fbxExporter->GetStatus().GetErrorString() << std::endl;
		}

        status = fbxExporter->Export(fbxScene);
        fbxExporter->Destroy();
    }

    void addAnimation(FbxScene* fbxScene, FbxManager* fbxManager) {
        // similar to the Unity version...
        FbxAnimCurveFilterUnroll filter;
        auto&& globalSettings = fbxScene->GetGlobalSettings();
        globalSettings.SetTimeMode(FbxTime::eFrames60);
        //FbxTime::EMode timeMode = globalSettings.GetTimeMode();

        //destroyAllAnimation(fbxScene);

        FbxAnimStack* animStack = FbxAnimStack::Create(fbxScene, "Base Stack");
        FbxAnimLayer* animLayer = FbxAnimLayer::Create(fbxScene, "Base Layer");
        animStack->AddMember(animLayer);
        FbxNode* fbxRootNode = fbxScene->GetRootNode();

        FbxAnimCurve* fbxAnimCurveX, * fbxAnimCurveY, * fbxAnimCurveZ;
        //rotation
        for (int i = 0; i < 24; i++) {
            FbxNode* fbxNode = fbxRootNode->FindChild(joints[i].c_str());
            if (i == 0) {
                fbxAnimCurveY = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
                set_curve_value(fbxAnimCurveY, npyArray, i, 1);
            }
            else {
                fbxAnimCurveX = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
                set_curve_value(fbxAnimCurveX, npyArray, i, 0);
                fbxAnimCurveY = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
                set_curve_value(fbxAnimCurveY, npyArray, i, 1);
                fbxAnimCurveZ = fbxNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
                set_curve_value(fbxAnimCurveZ, npyArray, i, 2);
            }
        }

        FbxAnimCurveNode* AnimCurveNode = fbxRootNode->LclRotation.GetCurveNode(animLayer, true);
        filter.Apply(*AnimCurveNode);

        for (int i = 0; i < 24; i++) {
            FbxNode* fbxNode = fbxRootNode->FindChild(joints[i].c_str());
            AnimCurveNode = fbxNode->LclRotation.GetCurveNode(animLayer, true);
            filter.Apply(*AnimCurveNode);
        }
         


        //translation
        FbxNode* fbxNodeTrans = fbxRootNode->FindChild("m_avg_Pelvis");
        FbxAnimCurve* fbxAnimCurve = fbxNodeTrans->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
        set_curve_value(fbxAnimCurve, translationArray, 0, 0);
        fbxAnimCurve = fbxNodeTrans->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
        set_curve_value(fbxAnimCurve, translationArray, 0, 1);
        fbxAnimCurve = fbxNodeTrans->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
        set_curve_value(fbxAnimCurve, translationArray, 0, 2);

        
        //filter.Apply(animStack);
    }

    void set_curve_value(FbxAnimCurve* curve, std::vector<std::vector<float>> rotArray, int jointNum, int coordNum) {
        // similar to the Unity version...
        int keyIndex = 0;
        FbxTime keyTime = FbxTime();
        keyTime.SetGlobalTimeMode(FbxTime::eFrames60);
        curve->KeyModifyBegin();
        for (int i = 0; i < totalFrame; i++) {
            keyTime.SetFrame(i, FbxTime::eFrames60);
            keyIndex = curve->KeyAdd(keyTime);
            curve->KeySetValue(keyIndex, rotArray[totalFrame * jointNum + i][coordNum]);
            curve->KeySetInterpolation(keyIndex, FbxAnimCurveDef::eInterpolationCubic);
        }
        curve->KeyModifyEnd();
    }

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void destroyAllAnimation(FbxScene* fbxScene) {
        int animStackCount = fbxScene->GetSrcObjectCount(FbxCriteria::ObjectType(FbxAnimStack::ClassId));
        for (int i = animStackCount - 1; i >= 0; --i) {
			FbxAnimStack* animStack = FbxCast<FbxAnimStack>(fbxScene->GetSrcObject(FbxCriteria::ObjectType(FbxAnimStack::ClassId), i));
            animStack->Destroy();
		}
    }

    std::vector<std::vector<float>> prepareNpy() {
        std::ifstream file(csvFilePath);

        if (!file.is_open()) throw std::runtime_error("Could not open file");

        std::string csvData((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        file.close();

        std::vector<std::string> rows = split(csvData, '\n');
        int numRows = rows.size() - 3;
        totalFrame = numRows / jointNumber;

        std::vector<std::string> firstRow = split(rows[0], ',');
        int numCols = firstRow.size();

        cout << numRows << " " << numCols << endl;

        std::vector<std::vector<float>> dataArray(numRows, std::vector<float>(numCols));

        for (int i = 0; i < numRows; i++) {
            std::vector<std::string> fields = split(rows[i], ',');
            for (int j = 0; j < numCols; j++) {
                try {
                    dataArray[i][j] = std::stof(fields[j]) * 57.29f;
                }
                catch (std::invalid_argument& e) {
                    std::cerr << "Error parsing CSV data at row " << i << ", column " << j << std::endl;
                    throw;
                }
            }
        }

        translationArray.resize(totalFrame, std::vector<float>(3));

        for (int i = 0; i < 3; i++) {
            std::vector<std::string> fields = split(rows[i + numRows], ',');
            for (int j = 0; j < totalFrame; j++) {
                try {
                    translationArray[j][i] = std::stof(fields[j]) * 57.29f;
                }
                catch (std::invalid_argument& e) {
                    std::cerr << "Error parsing CSV data at row " << i + numRows << ", column " << j << std::endl;
                    throw;
                }
            }
        }
        return dataArray;
    }
};


int main() {
	FBXController* exporter = new FBXController();
    exporter->Start();
	return 0;
}
