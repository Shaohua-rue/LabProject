#ifndef READ_DATA_H
#define READ_DATA_H

#include <iostream>
#include <vector>

#include <QString>
#include <QDebug>
#include <QFile>

class readData
{
public:
    std::vector<float> read_csv_data(QString docPath)
    {
        std::vector<float> vertexs_data;
		
        QFile inputFile(docPath);

		if (inputFile.open(QIODevice::ReadOnly))
		{
            uchar* fpr = inputFile.map(0, inputFile.size());

            int subcnt = 0;
            char* substr;
            char* s = strdup((char*)fpr);

            while (substr = strsep(&s, "\n"))
            {
                subcnt = 0;
                char* lineSubStr;

                while (subcnt < 4)
                {
                    lineSubStr = strsep(&substr, ",");
                    vertexs_data.push_back(atof(lineSubStr));
                    ++subcnt;
                }
            }

			inputFile.close();
		}

		else
		{
			qDebug() << "Can't open the file!" << endl;
		}

		return vertexs_data;
	}

    char* strsep(char** stringp, const char* delim)
    {
        char* s;
        const char* spanp;
        int c, sc;
        char* tok;
        if ((s = *stringp) == NULL)
            return (NULL);
        for (tok = s;;) {
            c = *s++;
            spanp = delim;
            do {
                if ((sc = *spanp++) == c) {
                    if (c == 0)
                        s = NULL;
                    else
                        s[-1] = 0;
                    *stringp = s;
                    return (tok);
                }
            } while (sc != 0);
        }
        /* NOTREACHED */
    }
};
#endif
