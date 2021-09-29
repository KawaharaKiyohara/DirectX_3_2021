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
    
    // step-1 スケルトンデータをロードして、モデルと関連付けする
    // スケルトンをロード。
    Skeleton skeleton;
    skeleton.Init("Assets/modelData/unityChan.tks");

    // モデルの初期化情報を構築
    ModelInitData modelInitData;
    // tkmファイルのファイルパスを指定する。
    modelInitData.m_tkmFilePath = "Assets/modelData/unityChan.tkm";
    // シェーダーファイルのファイルパスを指定する。
    modelInitData.m_fxFilePath = "Assets/shader/model.fx";
    // ノンスキンメッシュ用の頂点シェーダーのエントリーポイントを指定する。
    modelInitData.m_vsEntryPointFunc = "VSMain";
    // スキンメッシュ用の頂点シェーダーのエントリーポイントを指定。
    modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
    // ユニティちゃんはアニメーションデータでY-Upに補正されているので、
    // 上方向をY-Upに変更しておく。
    modelInitData.m_modelUpAxis = enModelUpAxisY;
    // スケルトンを指定する。
    modelInitData.m_skeleton = &skeleton;
    
    // 初期化情報を使って、モデルを初期化。
    Model model;
    model.Init(modelInitData);


    // step-2 アニメーションクリップをロードする。
    // 今回は「走りアニメーション」と「ジャンプアニメーション」の二つのアニメーションをロードするので、
    // AnimationClipの配列の要素数を２とする。
    AnimationClip animClip[2];
    // ジャンプアニメーションをロードする。
    animClip[0].Load("Assets/animData/jump.tka");
    // 走りアニメーションをロードする。
    animClip[1].Load("Assets/animData/run.tka");
    // 走りアニメーションはループ再生をしたいので、ループフラグをtrueにする。
    animClip[1].SetLoopFlag(true);

    // step-3 アニメーションの再生処理を初期化する。
    Animation animation;
    animation.Init(
        skeleton,       // アニメーションさせるスケルトン
        animClip,       // アニメーションクリップの配列
        2               // アニメーションクリップの数。
    );

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

        // step-4 コントローラーの入力で、再生するアニメーションを切り替える。
        if (g_pad[0]->IsTrigger(enButtonA)) {
            // Aボタンが押されたら、ジャンプアニメーションに変更する。
            // Animation::Play()関数に引数は、再生したいアニメーションクリップの番号。
            // この番号はAnimation::Init()関数に指定した、アニメーションクリップの配列の順番と同じとなる。
            animation.Play(0);
        }
        if (g_pad[0]->IsTrigger(enButtonB)) {
            // Bボタンが押されたら、走りアニメーションに変更する。
            // Animation::Play()関数に引数は、再生したいアニメーションクリップの番号。
            // この番号はAnimation::Init()関数に指定した、アニメーションクリップの配列の順番と同じとなる。
            animation.Play(1);
        }


        // step-5 アニメーションの再生を進める。
        animation.Progress(1.0f / 60.0f);

        // step-6 スケルトンを更新して、モデルをドローする
        skeleton.Update(model.GetWorldMatrix());
        model.Draw(renderContext);

        /////////////////////////////////////////
        // 絵を描くコードを書くのはここまで！！！
        //////////////////////////////////////
        // 1フレーム終了
        g_engine->EndFrame();
    }
    return 0;
}

