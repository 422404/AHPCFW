unsigned long long __aeabi_uidiv (unsigned long long numerator, unsigned long long denominator){
	unsigned long long result;
	result = 0;
	while ((numerator-denominator)>denominator){
		result++;
		numerator -=denominator;
	}
	return result;
}