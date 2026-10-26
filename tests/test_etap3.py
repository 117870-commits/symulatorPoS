import pathlib, subprocess, tempfile, unittest
ROOT = pathlib.Path(__file__).resolve().parents[1]
class Etap3(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.d = pathlib.Path(cls.tmp.name)
        cls.exe = cls.d / 'symulator.exe'
        subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-pedantic',*[str(ROOT / f) for f in ('main.cpp','Walidator.cpp','Pliki.cpp','Symulator.cpp')],'-o',str(cls.exe)], check=True)
    @classmethod
    def tearDownClass(cls): cls.tmp.cleanup()
    def run_case(self, config, trans):
        (self.d/'konfiguracja.txt').write_text(config, encoding='utf-8')
        (self.d/'zgloszenia.txt').write_text(trans, encoding='utf-8')
        return subprocess.run([str(self.exe)], cwd=self.d, capture_output=True, text=True)
    def test_kolejnosc_fifo_i_fee(self):
        config = '[symulacja]\nziarno = 12345\n[walidator]\nid = 1\nstake = 10\n'
        trans = ''.join(f'[transakcja]\nid = {i}\nnadejscie = 0\nczas_wykonania = 2\nfee = {fee}\ndeadline = 10\n' for i,fee in [(1,1),(2,9),(3,4)])
        result=self.run_case(config,trans)
        self.assertEqual(result.returncode,0,result.stderr)
        def ids(name):
            import re
            return [int(x) for x in re.findall(r'^id_transakcji\s*=\s*(\d+)',(self.d/name).read_text(encoding='utf-8'),re.M)]
        self.assertEqual(ids('wyniki_fifo.txt'),[1,2,3])
        self.assertEqual(ids('wyniki_fee.txt'),[2,3,1])
    def test_powtarzalnosc_remisy_i_wielu_walidatorow(self):
        import re
        config = '[symulacja]\nziarno=22\n[walidator]\nid=2\nstake=30\n[walidator]\nid=1\nstake=10\n'
        trans = ''.join(f'[transakcja]\nid={i}\nnadejscie={arrival}\nczas_wykonania=2\nfee={fee}\ndeadline=2\n' for i,arrival,fee in [(5,0,1),(3,0,9),(1,0,9),(4,1,9),(2,0,9)])
        r=self.run_case(config, trans)
        self.assertEqual(r.returncode,0,r.stderr)
        before=[(self.d/n).read_text(encoding='utf-8') for n in ('wyniki_fifo.txt','wyniki_fee.txt')]
        self.assertEqual(self.run_case(config,trans).returncode,0)
        self.assertEqual(before,[(self.d/n).read_text(encoding='utf-8') for n in ('wyniki_fifo.txt','wyniki_fee.txt')])
        blocks=before[1].split('[wynik]')[1:]
        records=[dict(re.findall(r'^(\w+)\s*=\s*(\w+)', b, re.M)) for b in blocks]
        self.assertEqual({int(x['id_transakcji']) for x in records},{1,2,3,4,5})
        self.assertEqual({int(x['id_transakcji']) for x in records[:2]},{1,2})
        self.assertEqual({int(x['id_transakcji']) for x in records[2:4]},{3,4})
        for validator in ('1','2'):
            jobs=sorted((int(x['start']),int(x['koniec'])) for x in records if x['id_walidatora']==validator)
            self.assertTrue(all(a[1]<=b[0] for a,b in zip(jobs,jobs[1:])))
        self.assertTrue(any(x['terminowo']=='nie' for x in records))
    def test_puste_i_walidacja(self):
        config='[symulacja]\nziarno=1\n[walidator]\nid=1\nstake=10\n'
        self.assertEqual(self.run_case(config,'# brak transakcji\n').returncode,0)
        self.assertNotIn('[wynik]',(self.d/'wyniki_fifo.txt').read_text())
        invalid=['[transakcja]\nid=1\nnadejscie=0\nczas_wykonania=0\nfee=1\ndeadline=10\n',
                 '[transakcja]\nid=1\nid=2\n', '[transakcja]\nid=1\n', '[nieznane]\nx=1\n',
                 '[transakcja]\nid=1 xyz\n']
        for data in invalid:
            r=self.run_case(config,data)
            self.assertNotEqual(r.returncode,0)
            self.assertIn('zgloszenia.txt',r.stderr)
        for bad in ('[symulacja]\nziarno=-1\n[walidator]\nid=1\nstake=1\n',
                    config+'[walidator]\nid=1\nstake=10\n',
                    '[symulacja]\nziarno=1\n[walidator]\nid=1\nstake=0\n'):
            self.assertNotEqual(self.run_case(bad,'').returncode,0)
if __name__ == '__main__': unittest.main()
