#include "HDRTexture.h"
#include "Graphics/Graphics.h"

HDRTexture::HDRTexture()
{
	width = 1280;
	height = 720;
}

HDRTexture::~HDRTexture()
{
	samplerState.Reset();
	SRV.Reset();
	texture2d.Reset();
}

bool HDRTexture::Load(const char* filename)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	FILE* fd;
	fopen_s(&fd, filename, "rb");
	if (fd == NULL) return false;

	// ヘッダーを調べる
	HDR_CheckHeader(fd);

	// コンバートする
	float* buf = new float[width * height * 4];
	HDR_ReadPixels(fd, buf);
	fclose(fd);

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	device->CreateTexture2D(&desc, NULL, texture2d.GetAddressOf());
	dc->UpdateSubresource(texture2d.Get(), 0, 0, buf, sizeof(float) * 4 * width, 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(texture2d.Get(), &srvDesc, SRV.ReleaseAndGetAddressOf());
	ID3D11ShaderResourceView* srv = SRV.Get();
	dc->GenerateMips(srv);

	delete[] buf;

	{
		D3D11_SAMPLER_DESC ssdesc = {};
		ssdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		// 左右は繰り返し、上下は繰り返し無し
		ssdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		ssdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		ssdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		ssdesc.MaxAnisotropy = (device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? D3D11_MAX_MAXANISOTROPY : 2u;

		ssdesc.MaxLOD = FLT_MAX;
		ssdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		HRESULT hr = device->CreateSamplerState(&ssdesc, samplerState.GetAddressOf());

		//サンプラーステートのSlot1にセット
		dc->PSSetSamplers(1, 1, samplerState.GetAddressOf());
	}

	return true;
}

void HDRTexture::Set(int slot)
{
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	ID3D11ShaderResourceView* srvs[1] = { SRV.Get() };
	dc->PSSetShaderResources(slot, 1, srvs);
}


//1118 HDR読み込み
void HDRTexture::HDR_CheckHeader(FILE* fp)
{
	char  buf[256];

	// ヘッダチェック
	for (;;) {
		if (fgets(buf, 256, fp) == NULL) return;
		if (buf[0] == '\n' || buf[0] == '\r') break;

		if (strncmp(buf, "#?", 2) == 0)
		{
			//if (strncmp(buf, "#?RADIANCE", 10) != 0) return -1;
		}
		//	フォーマットチェック
		if (strncmp(buf, "FORMAT=", 7) == 0)
		{
			//if( strncmp( buf, "FORMAT=32-bit_rle_rgbe", 22) != 0 ) return -1;
		}
	}

	// 画像サイズ取得
	if (fgets(buf, 256, fp) == NULL) return;
	char X[5], Y[5];
	sscanf_s(buf, "%s %d %s %d", Y, 5, &height, X, 5, &width);
}

bool HDRTexture::HDR_ReadLine(BYTE* scanline, FILE* fp)
{
	// ライン情報読み込み
	int val0 = getc(fp);	// 0x02
	int val1 = getc(fp);	// 0x02
	int val2 = getc(fp);	// size
	int val3 = getc(fp);	// size
	// 幅チェック
	if ((val2 << 8 | val3) != width) return false;

	for (int ch = 0; ch < 4; ch++)
	{
		for (int x = 0; x < width; )
		{
			int length = getc(fp);
			if (length == EOF) return false;

			bool Runlength = false;
			int val = 0;
			if (length > 128) {
				// ランレングス設定
				Runlength = true;
				length -= 128;
				val = getc(fp);
			}
			//	データ読み込み
			while (length > 0) {
				if (Runlength) scanline[x * 4 + ch] = val;
				else scanline[x * 4 + ch] = getc(fp);
				x++;
				length--;
			}
		}
	}
	return true;
}

bool HDRTexture::HDR_ReadPixels(FILE* fp, float* buf)
{
	int ret = 0;
	BYTE scanin[8192 * 4];
	float* work = buf;

	for (int y = height - 1; y >= 0; y--) {
		if (!HDR_ReadLine(scanin, fp)) return false;
		for (int x = 0; x < width; x++) {
			float exp = powf(2, scanin[x * 4 + 3] - 128.0f);
			work[0] = scanin[x * 4 + 0] * exp;
			work[1] = scanin[x * 4 + 1] * exp;
			work[2] = scanin[x * 4 + 2] * exp;
			work[3] = 1;
			work += 4;
		}
	}
	return true;
}
