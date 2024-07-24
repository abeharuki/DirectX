#include "LineBox.h"

LineBox* LineBox::Create()
{
	LineBox* linebox = new LineBox();
	linebox->Initialize();
	return linebox;
}

LineBox* LineBox::Create(AABB aabb)
{
	LineBox* linebox = new LineBox();
	linebox->Initialize(aabb);
	return linebox;
}

LineBox* LineBox::Create(OBB obb)
{
	LineBox* linebox = new LineBox();
	linebox->Initialize(obb);
	return linebox;
}

void LineBox::Initialize() {
	aabb_ = { {0,0,0},{0,0,0} };

	vertex[0] = { aabb_.min.x ,aabb_.min.y ,aabb_.min.z };
	vertex[1] = { aabb_.min.x ,aabb_.min.y ,aabb_.max.z };

	vertex[2] = { aabb_.max.x ,aabb_.min.y ,aabb_.min.z };
	vertex[3] = { aabb_.max.x ,aabb_.min.y ,aabb_.max.z };

	vertex[4] = { aabb_.min.x,aabb_.max.y , aabb_.min.z };
	vertex[5] = { aabb_.min.x,aabb_.max.y , aabb_.max.z };

	vertex[6] = { aabb_.max.x,aabb_.max.y , aabb_.min.z };
	vertex[7] = { aabb_.max.x,aabb_.max.y , aabb_.max.z };

	for (int i = 0; i < 12; ++i) {
		lines_[i] = std::make_unique<Line>();
	}

	lines_[0].reset(Line::CreateLine(vertex[0], vertex[1]));
	lines_[1].reset(Line::CreateLine(vertex[0], vertex[2]));
	lines_[2].reset(Line::CreateLine(vertex[1], vertex[3]));
	lines_[3].reset(Line::CreateLine(vertex[2], vertex[3]));

	lines_[4].reset(Line::CreateLine(vertex[4], vertex[5]));
	lines_[5].reset(Line::CreateLine(vertex[4], vertex[6]));
	lines_[6].reset(Line::CreateLine(vertex[5], vertex[7]));
	lines_[7].reset(Line::CreateLine(vertex[6], vertex[7]));

	lines_[8].reset(Line::CreateLine(vertex[0], vertex[4]));
	lines_[9].reset(Line::CreateLine(vertex[1], vertex[5]));
	lines_[10].reset(Line::CreateLine(vertex[2], vertex[6]));
	lines_[11].reset(Line::CreateLine(vertex[3], vertex[7]));
}

void LineBox::Initialize(AABB aabb) {
	worldTransform_.Initialize();
	aabb_ = aabb;
	vertex[0] = { aabb.min.x ,aabb.min.y ,aabb.min.z };
	vertex[1] = { aabb.min.x ,aabb.min.y ,aabb.max.z };

	vertex[2] = { aabb.max.x ,aabb.min.y ,aabb.min.z };
	vertex[3] = { aabb.max.x ,aabb.min.y ,aabb.max.z };

	vertex[4] = { aabb.min.x,aabb.max.y , aabb.min.z };
	vertex[5] = { aabb.min.x,aabb.max.y , aabb.max.z };

	vertex[6] = { aabb.max.x,aabb.max.y , aabb.min.z };
	vertex[7] = { aabb.max.x,aabb.max.y , aabb.max.z };

	for (int i = 0; i < 12; ++i) {
		lines_[i] = std::make_unique<Line>();
	}

	lines_[0].reset(Line::CreateLine(vertex[0], vertex[1]));
	lines_[1].reset(Line::CreateLine(vertex[0], vertex[2]));
	lines_[2].reset(Line::CreateLine(vertex[1], vertex[3]));
	lines_[3].reset(Line::CreateLine(vertex[2], vertex[3]));

	lines_[4].reset(Line::CreateLine(vertex[4], vertex[5]));
	lines_[5].reset(Line::CreateLine(vertex[4], vertex[6]));
	lines_[6].reset(Line::CreateLine(vertex[5], vertex[7]));
	lines_[7].reset(Line::CreateLine(vertex[6], vertex[7]));

	lines_[8].reset(Line::CreateLine(vertex[0], vertex[4]));
	lines_[9].reset(Line::CreateLine(vertex[1], vertex[5]));
	lines_[10].reset(Line::CreateLine(vertex[2], vertex[6]));
	lines_[11].reset(Line::CreateLine(vertex[3], vertex[7]));
}

void LineBox::Initialize(OBB obb) {
	obb_ = obb;

	vertex[0] = obb.center - (obb.size.x * obb.orientations[0]) - (obb.size.y * obb.orientations[1]) - (obb.size.z * obb.orientations[2]);
	vertex[1] = obb.center - (obb.size.x * obb.orientations[0]) - (obb.size.y * obb.orientations[1]) + (obb.size.z * obb.orientations[2]);

	vertex[2] = obb.center - (obb.size.x * obb.orientations[0]) + (obb.size.y * obb.orientations[1]) - (obb.size.z * obb.orientations[2]);
	vertex[3] = obb.center - (obb.size.x * obb.orientations[0]) + (obb.size.y * obb.orientations[1]) + (obb.size.z * obb.orientations[2]);

	vertex[4] = obb.center + (obb.size.x * obb.orientations[0]) - (obb.size.y * obb.orientations[1]) - (obb.size.z * obb.orientations[2]);
	vertex[5] = obb.center + (obb.size.x * obb.orientations[0]) - (obb.size.y * obb.orientations[1]) + (obb.size.z * obb.orientations[2]);

	vertex[6] = obb.center + (obb.size.x * obb.orientations[0]) + (obb.size.y * obb.orientations[1]) - (obb.size.z * obb.orientations[2]);
	vertex[7] = obb.center + (obb.size.x * obb.orientations[0]) + (obb.size.y * obb.orientations[1]) + (obb.size.z * obb.orientations[2]);

	for (int i = 0; i < 12; ++i) {
		lines_[i] = std::make_unique<Line>();
	}

	lines_[0].reset(Line::CreateLine(vertex[0], vertex[1]));
	lines_[1].reset(Line::CreateLine(vertex[0], vertex[2]));
	lines_[2].reset(Line::CreateLine(vertex[1], vertex[3]));
	lines_[3].reset(Line::CreateLine(vertex[2], vertex[3]));

	lines_[4].reset(Line::CreateLine(vertex[4], vertex[5]));
	lines_[5].reset(Line::CreateLine(vertex[4], vertex[6]));
	lines_[6].reset(Line::CreateLine(vertex[5], vertex[7]));
	lines_[7].reset(Line::CreateLine(vertex[6], vertex[7]));

	lines_[8].reset(Line::CreateLine(vertex[0], vertex[4]));
	lines_[9].reset(Line::CreateLine(vertex[1], vertex[5]));
	lines_[10].reset(Line::CreateLine(vertex[2], vertex[6]));
	lines_[11].reset(Line::CreateLine(vertex[3], vertex[7]));
}

void LineBox::Update(AABB aabb) {
	aabb_ = aabb;
	vertex[0] = { aabb_.min.x ,aabb_.min.y ,aabb_.min.z };
	vertex[1] = { aabb_.min.x ,aabb_.min.y ,aabb_.max.z };

	vertex[2] = { aabb_.max.x ,aabb_.min.y ,aabb_.min.z };
	vertex[3] = { aabb_.max.x ,aabb_.min.y ,aabb_.max.z };

	vertex[4] = { aabb_.min.x,aabb_.max.y , aabb_.min.z };
	vertex[5] = { aabb_.min.x,aabb_.max.y , aabb_.max.z };

	vertex[6] = { aabb_.max.x,aabb_.max.y , aabb_.min.z };
	vertex[7] = { aabb_.max.x,aabb_.max.y , aabb_.max.z };


	lines_[0]->SetLinePos(vertex[0], vertex[1]);
	lines_[1]->SetLinePos(vertex[0], vertex[2]);
	lines_[2]->SetLinePos(vertex[1], vertex[3]);
	lines_[3]->SetLinePos(vertex[2], vertex[3]);

	lines_[4]->SetLinePos(vertex[4], vertex[5]);
	lines_[5]->SetLinePos(vertex[4], vertex[6]);
	lines_[6]->SetLinePos(vertex[5], vertex[7]);
	lines_[7]->SetLinePos(vertex[6], vertex[7]);

	lines_[8]->SetLinePos(vertex[0], vertex[4]);
	lines_[9]->SetLinePos(vertex[1], vertex[5]);
	lines_[10]->SetLinePos(vertex[2], vertex[6]);
	lines_[11]->SetLinePos(vertex[3], vertex[7]);


}

void LineBox::Updata() {
	aabb_.min = { aabb_.min.x + obb_.center.x,aabb_.min.y + obb_.center.y,aabb_.min.z + obb_.center.z };
	aabb_.max = { aabb_.max.x + obb_.center.x,aabb_.max.y + obb_.center.y,aabb_.max.z + obb_.center.z };
	vertex[0] = { aabb_.min.x ,aabb_.min.y ,aabb_.min.z };
	vertex[1] = { aabb_.min.x ,aabb_.min.y ,aabb_.max.z };

	vertex[2] = { aabb_.max.x ,aabb_.min.y ,aabb_.min.z };
	vertex[3] = { aabb_.max.x ,aabb_.min.y ,aabb_.max.z };

	vertex[4] = { aabb_.min.x,aabb_.max.y , aabb_.min.z };
	vertex[5] = { aabb_.min.x,aabb_.max.y , aabb_.max.z };

	vertex[6] = { aabb_.max.x,aabb_.max.y , aabb_.min.z };
	vertex[7] = { aabb_.max.x,aabb_.max.y , aabb_.max.z };


	lines_[0]->SetLinePos(vertex[0], vertex[1]);
	lines_[1]->SetLinePos(vertex[0], vertex[2]);
	lines_[2]->SetLinePos(vertex[1], vertex[3]);
	lines_[3]->SetLinePos(vertex[2], vertex[3]);

	lines_[4]->SetLinePos(vertex[4], vertex[5]);
	lines_[5]->SetLinePos(vertex[4], vertex[6]);
	lines_[6]->SetLinePos(vertex[5], vertex[7]);
	lines_[7]->SetLinePos(vertex[6], vertex[7]);

	lines_[8]->SetLinePos(vertex[0], vertex[4]);
	lines_[9]->SetLinePos(vertex[1], vertex[5]);
	lines_[10]->SetLinePos(vertex[2], vertex[6]);
	lines_[11]->SetLinePos(vertex[3], vertex[7]);

}

void LineBox::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool flag) {

	for (int i = 0; i < 12; ++i) {
		lines_[i]->Draw(worldTransform, viewProjection, false);
	}
}

void LineBox::SetColor(Vector4 color)
{
	for (int i = 0; i < 12; ++i) {
		lines_[i]->SetColor(color);
	}

}
