#ifndef DOMAIN_H
#define DOMAIN_H


class Domain
{
    private:

        int _min;
        int _max;

    public:

        Domain(int min, int max);
        bool isIncluded(int n);
        virtual ~Domain();

};

#endif // DOMAIN_H
