/*!
 * @brief	シンプルなモデルシェーダー。
 */


////////////////////////////////////////////////
// 定数バッファ。
////////////////////////////////////////////////
//モデル用の定数バッファ
cbuffer ModelCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////
//スキニング用の頂点データをひとまとめ。
struct SSkinVSIn{
	
};
//頂点シェーダーへの入力。
struct SVSIn{
	float4 pos 		: POSITION;		//モデルの頂点座標。
	float2 uv 		: TEXCOORD0;	//UV座標。
	// step-1 頂点シェーダーへの入力に、スキンインデックスとスキンウェイトを追加。
	int4 skinIndex	: BLENDINDICES0;		// スキンインデックス。
	float4 skinWeight	 : BLENDWEIGHT0;	// スキンウェイト。
};
//ピクセルシェーダーへの入力。
struct SPSIn{
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float2 uv 			: TEXCOORD0;	//uv座標。
};

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
Texture2D<float4> g_albedo : register(t0);				//アルベドマップ

// step-2 ボーン行列の配列にアクセスする変数を追加。
StructuredBuffer<float4x4> g_boneMatrix : register(t3);	//ボーン行列。

sampler g_sampler : register(s0);	//サンプラステート。

////////////////////////////////////////////////
// 関数定義。
////////////////////////////////////////////////

/// <summary>
/// スキンなしメッシュ用の頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSMain(SVSIn vsIn)
{
	SPSIn psIn;

	psIn.pos = mul(mWorld, vsIn.pos);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);

	psIn.uv = vsIn.uv;
	return psIn;
}
/// <summary>
/// スキンありメッシュの頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSSkinMain( SVSIn vsIn ) 
{
	SPSIn psIn = (SPSIn)0;
	float4x4 m = 0;

	// step-3 ボーン行列からワールド行列を計算する。
	float weightTotal = 0.0f;
	// このループは、骨４本ではなく３本目までのループ。
	[unroll]
    for (int i = 0; i < 3; i++)
    {
		// スキンインデックスから関連付けされているボーン行列を取得。
		float4x4 mBoneMatrix = g_boneMatrix[ vsIn.skinIndex[i] ];
		// ボーン行列に重みを乗算して、ワールド行列に足し算。
        m += mBoneMatrix * vsIn.skinWeight[i];
		// ここまでのウェイトの合計を計算する。
        weightTotal += vsIn.skinWeight[i];
    }
    // 4本目。ウェイトの合計が必ず1.0になるように、
	// 4本目のウェイトは、ここまでのウェイトの合計値から計算している。
    m += g_boneMatrix[ vsIn.skinIndex[3] ] * (1.0f - weightTotal);

	psIn.pos = mul(m, vsIn.pos);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);

	psIn.uv = vsIn.uv;

	return psIn;
}
/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain( SPSIn psIn ) : SV_Target0
{
	float4 albedoColor = g_albedo.Sample(g_sampler, psIn.uv);
	return albedoColor;
}
