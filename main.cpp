#include <Novice.h>
#include "GeometryUtility.h"
#include <imgui.h>

const char kWindowTitle[] = "学籍番号";

GeometryUtility geometryUtility;

struct  Ball
{
	Vector3 position;
	Vector3 velocity;
	Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

struct Capsule
{
	Segment segment;
	float radius;
};

Vector3 Reflect(const Vector3& input, const Vector3& normal) {

	Vector3 proj = geometryUtility.Project(input, normal);

	Vector3 reflection = geometryUtility.Subtract(input, geometryUtility.Multiply(2.0f, proj));
	
	return reflection;

}

bool IsCollision(const Sphere& sphere, const Plane& plane) {

	float distance = geometryUtility.Dot(plane.normal, sphere.center) - plane.distance;
	Novice::ScreenPrintf(0,0,"Distance: %f\n", distance);
	if (fabs(distance) <= sphere.radius) {
		return true;
	}
	return false;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	int kWindowWidth = 1280;
	int kWindowHeight = 720;

	float radius = 10.0f;
	float theta = -4.90f;
	float phi = 3.12f;

	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };

	Plane plane;
	plane.normal = geometryUtility.normalize({ -0.02f,0.9f,-0.3f });
	plane.distance = 0.0f;

	Ball ball{};
	ball.position = { 0.8f,1.2f,0.3f };
	ball.acceleration = { 0.0f,-9.8f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = 0xFFFFFFFF;

	float deltaTime = 1.0f / 60.0f;

	float e = 0.5f;

	bool stan = false;


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		ball.velocity = geometryUtility.Add(geometryUtility.Multiply(deltaTime, ball.acceleration), ball.velocity);
		ball.position = geometryUtility.Add(geometryUtility.Multiply(deltaTime, ball.velocity), ball.position);

		

		if (IsCollision(Sphere{ ball.position,ball.radius }, plane)) {
			Vector3 reflected = Reflect(ball.velocity, plane.normal);
			Vector3 projectToNormal = geometryUtility.Project(reflected, plane.normal);
			Vector3 movingDirection = geometryUtility.Subtract(reflected,projectToNormal);
			ball.velocity = geometryUtility.Add(geometryUtility.Multiply(e,projectToNormal),movingDirection);
			
			float distance = geometryUtility.Dot(plane.normal, ball.position) - plane.distance;
			ball.position = geometryUtility.Add(ball.position, geometryUtility.Multiply((ball.radius - distance), plane.normal));
		}

		Vector3 cameraPosition = geometryUtility.SphericalToCartesian(radius, theta, phi);
		Vector3 cameraTarget = { 0.0f, 0.0f, 0.0f };
		Vector3 cameraUp = { 0.0f, 1.0f, 0.0f };

		Matrix4x4 viewMatrix = geometryUtility.MakeLookAtMatrix4x4(cameraPosition, cameraTarget, cameraUp);
		Matrix4x4 worldMatrix = geometryUtility.MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 projectionMatrix = geometryUtility.MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = geometryUtility.Multiply(worldMatrix, geometryUtility.Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewProjectionMatrix = geometryUtility.Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = geometryUtility.MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///


		geometryUtility.DrawGrid(viewProjectionMatrix, viewportMatrix);

		geometryUtility.DrawPlane(plane, viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);
		geometryUtility.DrawSphere({ ball.position,ball.radius }, viewProjectionMatrix, viewportMatrix, ball.color);

		ImGui::Begin("Window");
		ImGui::DragFloat("theta", &theta, 0.01f);
		ImGui::DragFloat("phi", &phi, 0.01f);
		//ImGui::Text("Uniform Circular Motion");
		//if (ImGui::Button("Start")) {
		//	isMoving = true;
		//}
		//ImGui::SameLine();
		//if (ImGui::Button("Stop")) {
		//	isMoving = false;
		//}
		//ImGui::SliderFloat("length", &conicalPendulum.length, 0.00f, 1.0f);
		//ImGui::SliderFloat("halfApexAngle", &conicalPendulum.halfApexAngle, 0.0f, 1.0f);
		ImGui::End();


		Novice::ScreenPrintf(0, 20, "%d", stan);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
