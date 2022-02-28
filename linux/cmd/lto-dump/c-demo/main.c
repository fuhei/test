static int ret = 1024;

int func1(void)
{
	return ret;
}

int main(int argc, char *argv[])
{
	return func1();
}
