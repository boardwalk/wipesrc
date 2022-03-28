/*the flip function to flip the Gte values for RidgeRacer */

void FlipRotMatrixX()
{
	MATRIX	RMat;

	ReadRotMatrix(&RMat);

	RMat.m[0][0] = -RMat.m[0][0];
	RMat.m[0][1] = -RMat.m[0][1];
	RMat.m[0][2] = -RMat.m[0][2];

	SetRotMatrix(&RMat);
}

void FlipTRX()
{
	MATRIX	RMat;

	ReadRotMatrix(&RMat);
	RMat.t[0] = -RMat.t[0];
	SetTransMatrix(&RMat);
}

