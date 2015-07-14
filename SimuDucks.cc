#include <iostream>
#include <vector>

class QuackObservable;
class Quackable;

class Observer {
public:
    void virtual update(QuackObservable *duck)=0;
};

class QuackObservable {
public:
    void virtual registerObserver(Observer *observer) = 0;
    void virtual notifyObservers() = 0;
};

class LinerObservable : public QuackObservable {
    typedef std::vector<Observer *>::const_iterator const_iterator;
    std::vector<Observer *> observers;
    QuackObservable *duck;
public:
    LinerObservable(QuackObservable *duck):duck(duck){}

    void virtual registerObserver(Observer *observer) {
        observers.push_back(observer);
    }

    void virtual notifyObservers() {
        const_iterator iter;
        for (iter = observers.cbegin(); iter < observers.cend(); iter++) {
            (*iter)->update(duck);
        }
    }
};

class Quackable {
public:
    virtual void quack () = 0;
    virtual ~Quackable() = 0;
};

Quackable::~Quackable() {}

class MallardDuck : public Quackable, public QuackObservable {
    LinerObservable observable; // This layer of implicit should use to choose different observables.

public:
    MallardDuck () : observable(this) {}

    virtual void quack() {
        std::cout<<"Quack"<<std::endl;
        notifyObservers();
    }

    void virtual registerObserver(Observer *observer) {
        observable.registerObserver(observer);
    }

    void virtual notifyObservers() {
        observable.notifyObservers();
    }
};

class RedHeadDuck : public Quackable {
public:
    virtual void quack() {
        std::cout<<"Quack"<<std::endl;
    }
};

class DuckCall : public Quackable {
public:
    virtual void quack() {
        std::cout<<"Kwak"<<std::endl;
    }
};

class RubberDuck : public Quackable {
public:
    virtual void quack() {
        std::cout<<"Squeak"<<std::endl;
    }
};

class Goose {
public:
    virtual void honk() {
        std::cout<<"Honk"<<std::endl;
    }
};

class GooseAdaptor : public Quackable {
    Goose *goose_;
public:
    ~GooseAdaptor() {
        delete goose_;
    }

    GooseAdaptor(Goose *goose) {
        goose_ = goose;
    }

    virtual void quack() {
        goose_->honk();
    }
};

class QuackCounter : public Quackable {
    Quackable *duck_;
    static int numberOfQuack;
public:
    ~QuackCounter() {
        delete duck_;
    }

    QuackCounter(Quackable *duck) {
        duck_ = duck;
    }

    static int getQuacks() {
        return numberOfQuack;
    }

    virtual void quack() {
        numberOfQuack += 1;
        duck_->quack();
    }
};

int QuackCounter::numberOfQuack = 0;

class AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() = 0;
    virtual Quackable *createRedHeadDuck() = 0;
    virtual Quackable *createDuckCall() = 0;
    virtual Quackable *createRubberDuck() = 0;
};

class DuckFactory : public AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() {
        return new MallardDuck();
    }
    virtual Quackable *createRedHeadDuck() {
        return new RedHeadDuck();
    }
    virtual Quackable *createDuckCall() {
        return new DuckCall();
    }
    virtual Quackable *createRubberDuck() {
        return new RubberDuck();
    }
};

class CountingDuckFactory : public AbstractDuckFactory {
public:
    virtual Quackable *createMallardDuck() {
        return new QuackCounter(new MallardDuck());
    }
    virtual Quackable *createRedHeadDuck() {
        return new QuackCounter(new RedHeadDuck());
    }
    virtual Quackable *createDuckCall() {
        return new QuackCounter(new DuckCall());
    }
    virtual Quackable *createRubberDuck() {
        return new QuackCounter(new RubberDuck());
    }
};

class Flock : public Quackable {
    typedef std::vector<Quackable *>::const_iterator const_iterator;
    std::vector<Quackable *> quackers;
public:
    void add(Quackable *duck) {
        quackers.push_back(duck);
    }

    virtual void quack() {
        const_iterator iter;
        for (iter = quackers.cbegin(); iter < quackers.cend(); iter++) {
            (*iter)->quack();
        }
    }

};



class DuckSimulator {
public:
    void simulate(AbstractDuckFactory *factory);
    void simulate(Quackable *duck) {
        duck->quack();
    }
};

void DuckSimulator::simulate(AbstractDuckFactory *factory) {
    Quackable *mallardDuck = factory->createMallardDuck();
    Quackable *redHeadDuck = factory->createRedHeadDuck();
    Quackable *duckCall = factory->createDuckCall();
    Quackable *rubberDuck = factory->createRubberDuck();
    Quackable *goose = new GooseAdaptor(new Goose());

    std::cout<<"Duck Simulator"<<std::endl;

    Flock *flockOfDucks = new Flock();

    flockOfDucks->add(redHeadDuck);
    flockOfDucks->add(duckCall);
    flockOfDucks->add(rubberDuck);
    flockOfDucks->add(goose);

    Flock *flockOfMallards = new Flock();

    Quackable *mallardOne = factory->createMallardDuck();
    Quackable *mallardTwo = factory->createMallardDuck();
    Quackable *mallardThree = factory->createMallardDuck();
    Quackable *mallardFour = factory->createMallardDuck();

    flockOfMallards->add(mallardOne);
    flockOfMallards->add(mallardTwo);
    flockOfMallards->add(mallardThree);
    flockOfMallards->add(mallardFour);

    flockOfDucks->add(flockOfMallards);

    simulate(flockOfDucks);

    std::cout<<"The duck quacked "<<QuackCounter::getQuacks()<<" times"<<std::endl;

    delete flockOfMallards;
    delete flockOfDucks;
    delete mallardDuck;
    delete redHeadDuck;
    delete duckCall;
    delete rubberDuck;
    delete goose;
}

int main()
{
    DuckSimulator s1;
    s1.simulate(new CountingDuckFactory());
}
