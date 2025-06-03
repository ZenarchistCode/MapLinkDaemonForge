modded class Math
{
	protected static ref TIntArray m_QRandomNumbers = new TIntArray;

	// Adds a new array — shouldn't be called manually. Use UApi().CheckAndRenewQRandom();
	static void AddQRandomNumber(TIntArray numbers)
	{
		if (!m_QRandomNumbers)
		{
			m_QRandomNumbers = new TIntArray;
		}
		m_QRandomNumbers.InsertAll(numbers);
	}

	// Returns the remaining random numbers to choose from
	static int QRandomRemaining()
	{
		if (!m_QRandomNumbers)
		{
			return 0;
		}
		return m_QRandomNumbers.Count();
	}

	// Gets the number and removes it from the array
	protected static int GetAndRemoveNumber()
	{
		int idx = m_QRandomNumbers.GetRandomIndex();
		int number = m_QRandomNumbers.Get(idx);
		m_QRandomNumbers.Remove(idx);
		return number;
	}

	// Returns a random number between int.MIN and int.MAX
	static int QRandom()
	{
		if (QRandomRemaining() <= 0)
		{
			Print("[UAPI] Q RANDOM OUT OF NUMBERS USING VANILLA RANDOM");
			return RandomInt(int.MIN, int.MAX);
		}

		int number = GetAndRemoveNumber();
		return number;
	}

	// Returns a random integer — max difference between numbers is int.MAX (2147483647)
	// Unless returning a number between 0 and int.MAX exactly,
	// it's recommended not to exceed a difference of ~10,000 — use float instead
	static int QRandomInt(int min = 0, int max = int.MAX)
	{
		if (QRandomRemaining() <= 0)
		{
			Print("[UAPI] Q RANDOM OUT OF NUMBERS USING VANILLA RANDOM");
			return RandomInt(min, max);
		}

		if (min == max)
		{
			return min;
		}

		if (max < min)
		{
			int tmp = max;
			max = min;
			min = tmp;
		}

		int number = Math.AbsInt(GetAndRemoveNumber());

		if (min == 0 && max == int.MAX)
		{
			return number;
		}

		int diff = max - min;
		int randomNum = number % (diff + 1);

		return min + randomNum;
	}

	// Returns a random float
	static float QRandomFloat(float min = 0, float max = 1)
	{
		if (QRandomRemaining() <= 0)
		{
			Print("[UAPI] Q RANDOM OUT OF NUMBERS USING VANILLA RANDOM");
			return RandomFloat(min, max);
		}

		if (min == max)
		{
			return min;
		}

		if (max < min)
		{
			float tmp = max;
			max = min;
			min = tmp;
		}

		int number = Math.AbsInt(GetAndRemoveNumber());
		float num = number / int.MAX;
		float diff = max - min;
		float dnum = diff * num;

		return dnum + min;
	}

	// Returns a random true or false value
	static bool QRandomFlip()
	{
		if (QRandomRemaining() <= 0)
		{
			Print("[UAPI] Q RANDOM OUT OF NUMBERS USING VANILLA RANDOM");
			int retval = RandomInt(1, int.MAX) % 2;
			return (retval != 0);
		}

		int number = Math.AbsInt(GetAndRemoveNumber());
		int reval = number % 2;

		return (reval != 0);
	}
}