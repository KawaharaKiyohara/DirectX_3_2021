#include "stdafx.h"
#include "system/system.h"

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // ゲームの初期化
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    // ここから初期化を行うコードを記述する
    //////////////////////////////////////
    
    // スケルトンデータをロードする。
    Skeleton skeleton;
    skeleton.Init("Assets/modelData/unityChan.tks");

    // モデルデータをロードして、スケルトンと関連付けする。
    ModelInitData modelInitData;
    // tkmファイルのファイルパスを指定する。
    modelInitData.m_tkmFilePath = "Assets/modelData/unityChan.tkm";
    // シェーダーファイルのファイルパスを指定する。
    modelInitData.m_fxFilePath = "Assets/shader/model.fx";
    // ノンスキンメッシュ用の頂点シェーダーのエントリーポイントを指定する。
    modelInitData.m_vsEntryPointFunc = "VSMain";
    //【注目】スキンメッシュ用の頂点シェーダーのエントリーポイントを指定。
    modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
    //【注目】スケルトンを指定する。
    modelInitData.m_skeleton = &skeleton;
    
    // 初期化情報を使って、モデルを初期化。
    Model model;
    model.Init(modelInitData);
   
    // ユニティちゃんの回転クォータニオンを定義する。
    Quaternion qRotation;
    //////////////////////////////////////
    // 初期化を行うコードを書くのはここまで！！！
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ここからゲームループ
    while (DispatchWindowMessage())
    {
        // 1フレームの開始
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ここから絵を描くコードを記述する
        //////////////////////////////////////

        
        // ユニティちゃんを回転させる。
        // Y軸周りの回転クォータニオンを作成。
        qRotation.AddRotationY(g_pad[0]->GetRStickXF() * 0.01f );
        qRotation.AddRotationX(g_pad[0]->GetRStickYF() * 0.01f );
        // モデルのワールド行列を更新する。
        model.UpdateWorldMatrix(g_vec3Zero, qRotation, g_vec3One);

        // step-1 コントローラーの入力で頭の骨を上下に動かす。
        // 骨の名前でボーンIDを検索する。
        int boneId = skeleton.FindBoneID(L"Character1_Neck");
        // 検索したボーンIDを使って、ボーンを取得する。
        Bone* bone = skeleton.GetBone(boneId);
        // ボーンのローカル行列を取得する。
        Matrix boneMatrix = bone->GetLocalMatrix();
        // 【注目】コントローラーを使って、ローカル行列のZ方向の平行移動成分を変化させる。
        // なんでZが上なの？ユニティちゃんはz-upで作られているから。
        boneMatrix.m[3][2] += g_pad[0]->GetLStickYF();
        // 変更したボーン行列を設定する。
        bone->SetLocalMatrix(boneMatrix);

        // スケルトンを更新する。
        // Skeleton::Update()関数の中で、ワールド行列を計算している。
        skeleton.Update(model.GetWorldMatrix());

        // step-5 モデルをドロー。
        model.Draw(renderContext);

        /////////////////////////////////////////
        // 絵を描くコードを書くのはここまで！！！
        //////////////////////////////////////
        // 1フレーム終了
        g_engine->EndFrame();
    }
    return 0;
}

