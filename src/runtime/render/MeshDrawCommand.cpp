#include "MeshDrawCommand.h"

namespace shzk
{
	bool MeshDrawCommand::SubmitDraw()
	{
		if (SubmitDrawBegin())
		{
			SubmitDrawEnd();
			return true;
		}
		return false;
	}

	bool MeshDrawCommand::SubmitDrawBegin()
	{
		return true;
	}

	void MeshDrawCommand::SubmitDrawEnd()
	{
	}
}