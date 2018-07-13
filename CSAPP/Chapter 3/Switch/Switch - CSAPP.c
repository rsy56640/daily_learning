

void switch_eg(long x, long n, long* dest)
{
	long val = x;
	switch(n)
	{
		case 100:
			val *= 13;
			break;
			
		case 102:
			val += 10;
			/* fall through */
			
		case 103:
			val += 11;
			break;
			
		case 104:
		case 106:
			val *= val;
			break;
		
		default:
			val = 0;
	}
	*dest = val;
}


void switch_eg_impl(long x, long n, long* dest)
{
	static void* jt[7] =
	{
		&&loc_A, &&loc_def, &&loc_B,
		&&loc_C, &&loc_D, &&loc_def,
		&&loc_D
	};
	unsigned long index = n - 100;
	long val;
	if(index > 6)
		goto loc_def;
	/* Multiway branch */
	goto *jt[index];
	
	loc_A:	/* case 100 */
		val *= 13;
		goto done;
	
	loc_B:	/* case 102 */
		x = x + 10;
		/* fall through */
		
	loc_C:	/* case 103 */
		val += 11;
		goto done;
		
	loc_D:	/* case 104, 106 */
		val = x * x;
		goto done;
	
	loc_def:	/* Default case */
		val = 0;
	
	done:
		*dest = val;
}

int test_switch(long n)
{
	int i = 0;
    switch(n)
	{
        case 4: i = 4;break;
        case 10: i = 10;break;
        case 50: i = 50;break;
        case 100: i = 100;break;
        case 200: i = 200;break;
        case 500: i = 500;break;
        default: i = 0;break;
    }
    return i;
}

int main(int, char**)
{
	
	
	return 0;
}
