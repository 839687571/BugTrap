http://blog.sina.com.cn/s/blog_648d306d0100qmca.html


Windowsƽ̨�µ�C++�����쳣ͨ���ɷ�Ϊ���֣�
�ṹ���쳣��Structured Exception���������Ϊ�����ϵͳ��ص��쳣����C++�쳣


����crash�� ��һ����δ������������쳣����Windows����ϵͳ�ṩ��һ��API���������ڳ���crash֮ǰ�л��ᴦ����Щ�쳣��
���� SetUnhandleExceptionFilter������
��C++Ҳ��һ�����ƺ���set_terminate���Դ���δ�������C++�쳣����


SetUnhandleExceptionFilter������������һ���Լ��ĺ�����Ϊȫ��SEH���˺�����
������crashǰ��������ǵĺ� �����д������ǿ������õ��� _EXCEPTION_POINTERS �ṹ���͵ı���ExceptionInfo��
�������˶��쳣�������Լ������쳣���߳�״̬��
���˺�������ͨ�����ز�ͬ��ֵ����ϵͳ�������л��˳�Ӧ�ó���

// Զ�̵��Է���:
SetUnhandledExceptionFilter + Minidump
//ȷ�����õ�dump�ļ���Դ���롢exe��pdb�ļ��汾��һ�µģ���Ҫ�����Ǳ���ά���ó���汾��Ϣ