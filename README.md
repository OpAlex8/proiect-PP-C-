  Proiectul se bazeaza pe imagini de tip BMP(bitmap), unde o imagine este un fisier binar, astfel fiind o imagine color RGB. Fiecare pixel are culoarea data de tripletul (R, G, B) cu valori pe fiecare canal intre 0 si 255. Dimensiunea imaginii este de 500 x 400 pixeli(latime x inaltime).
  Proiectul are in mare 2 parti:
  1.	criptarea si decriptarea unei imagini prin cifru simetric(un alogritm de criptare, un algoritm de decriptare si o cheie secreta comuna).
  Pasii pentru realizarea criptarii:
    •	initial, cu ajutorul unei chei generam numere intregi fara semn pe 32 de biti cu ajutorul generatorului XORSHIFT32 propus de George Marsaglia;
    •	folosind algoritmul lui Durstenfeld rearanjam ordinea pixelilor din imagine, doar ca in loc sa folosim numere random, o sa alegem numerele generate mai sus;
    •	Pentru a ne asigura ca imaginea este criptata cum trebuie si transmita correct, in caz ca este nevoie, calculam o cheie chi-patrat care sa ne spuna acest lucru.
  Pentru decriptarea imaginii urmam aceeasi pasi, doar ca in sens invers.
  2.	template matching asupra altei imagini de tip BMP.
  Avand o imagine cu cifre scrise in diferite moduri, trebuie sa identificam pe baza unor imagini sablon cifrele care se aseamana.
  Pasii pentru template matching:
    •	transformam imaginea si sabloanele in imagini greyscale;
    •	se gliseaza fiecare sablon pe intreaga imagine;
    •	se calculeaza procentul de similitudine la fiecare punct gasit;
    •	daca depaseste un prag minim setat la inceput este retinut;
    •	sortam punctele unde am gasit similitudini in functe de procent;
    •	eliminam chenarele care se suprapun mai mult decat un prag setat la inceput(in caz ca se intersecteaza mai mult decat pragul, cea cu procentul mai mic este scoasa);
    •	facem un chenar de o anumita culoare pentru fiecare cifra gasita.
   (0 cu rosu, 1 cu galben, 2 cu verde, …)
