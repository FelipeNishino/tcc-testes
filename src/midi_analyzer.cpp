#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>
#include "libjson/json.hpp"
#include "libmidifile/MidiEvent.hpp"
#include "libmidifile/MidiEventList.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include "midi.hpp"
#include "midi_analyzer.hpp"
#include "utils.hpp"
/*
NOTE ON/OFF
DELTA? EVENTCODE NOTA VELOCITY 
' = DECIMAL
*/

void MidiAnalyzer::display_note_ocurrence(TrackInfo track) {
    if (track.note_count == 0) {
        std::cout << "Track doesn't have any notes, skipping...\n";
        // return;
    }
    std::array<std::string, 13> labels={ "C", "c", "D", "d", "E", "F", "f", "G", "g", "A", "a", "B", "X"};
        for (auto note : {" ", "C", "c", "D", "d", "E", "F", "f", "G", "g", "A", "a", "B", "X"}) {
            std::cout << note << " ";
        }
        std::cout << "\n";
        int k = 0;
        for (auto d1 : track.note_matrix) {
            std::cout << labels[k] << " ";
            for (int val : d1) {
                std::cout << val << " ";
            }
            std::cout << "\n";
            k++;
        }
        std::cout << "\n";
}

EventType MidiAnalyzer::get_event_type(smf::MidiEvent ev) {
    if (ev.isNoteOn())
        return EventType::note_on;
    if (ev.isNoteOff())
        return EventType::note_off;
    if (ev.isTimeSignature())
        return EventType::time_signature;
    if (ev.isKeySignature())
        return EventType::key_signature;
    if (ev.isTempo())
        return EventType::tempo;
    return EventType::other;
}

MidiFeatures MidiAnalyzer::analyze(std::string midi_name) {
    MidiFeatures feat;
    TrackState st;
    smf::MidiFile midifile;
    smf::MidiEvent* linked_ev;
    std::_List_iterator<int> find_it;
    std::list<int> chord_list;
    int j = 1;
    std::set<int> unique_quarters;
    std::vector<TrackInfo> tracks;
    std::vector<SongEventCollections> ev_coll;
    Midi::read_midi(&midifile, midi_name);
    // midifile.joinTracks();

    if (midifile.isAbsoluteTicks()) {
        midifile.deltaTicks();
    }
    // if (midifile.isDeltaTicks()) {
        // midifile.absoluteTicks();
    // }

    // std::cout << "linked " << midifile.linkNotePairs() << " notes\n";
    std::cout << "linked " << midifile.linkEventPairs() << " events\n";

    midifile.doTimeAnalysis();
    // std::cout << "\nSong: " << midi_name << '\n';
    // std::cout << "Tracks: " << midifile.getTrackCount() << '\n';
    for (int i = 0; i < midifile.getTrackCount(); i++) {
        tracks.push_back(TrackInfo{});
        ev_coll.push_back(SongEventCollections{});
        // std::cout << "Event count: " << midifile[i].getEventCount() << '\n';
        j = 0;
        st.prev_on.set = false;
        st.prev_off.set = false;

        while (j < midifile[i].getEventCount()) {
            int current_quarter{};
            st.curr_ev.ev = &midifile[i][j];
            st.curr_ev.key = st.curr_ev.ev->getKeyNumber();
            switch (get_event_type(midifile[i][j])) {
                case note_on:
                    // std::cout << "Note on\n";
                    if (st.prev_on.set) {
                        // std::cout << "First note\n";
                        // j++;
                        // st.prev_note_type = on;
                        // continue;
                    
                        if (st.curr_ev.ev->tick == 0 && st.prev_note_type == on) {
                            if (chord_list.empty()) chord_list.push_back(st.prev_on.key);
                            chord_list.push_back(st.curr_ev.key);
                        }
                        else {
                            if (chord_list.size() == 1 && chord_list.front() == st.prev_on.key) chord_list.clear();
                            st.prev_on = st.curr_ev;
                        }
                    }
                    st.prev_on.ev = st.curr_ev.ev;
                    st.prev_on.key = st.curr_ev.key;
                    st.prev_on.set = true;
                    st.prev_note_type = on;
                    tracks[i].note_count++;
                    linked_ev = midifile[i][j].getLinkedEvent();
                    current_quarter = std::nearbyint(midifile[i][j].getTickDuration() / midifile.getTicksPerQuarterNote());
                    unique_quarters.insert(current_quarter);
                    
                    tracks[i].note_durations[midifile[i][j].getDurationInSeconds()]++;
                    ev_coll[i].note_on_evs.push_back(&midifile[i][j]);
                    break;
                case note_off:
                    // std::cout << "Note off\n";
                    find_it = std::find(chord_list.begin(), chord_list.end(), st.curr_ev.key);
                    if (find_it != chord_list.end()) {
                        // std::cout << "Nota acorde\n";
                        chord_list.erase(find_it);
                        if (chord_list.empty()) {
                            // std::cout << "Finalizou um acorde\n";
                            if (st.prev_off.set) {
                                // std::cout << "Último note off: nota " << st.prev_off.key % 12 << ", Tipo: " << (st.prev_off.component == chord ? "Acorde\n": "Nota\n");
                                tracks[i].note_matrix[(st.prev_off.component == chord ? 12 : st.prev_off.key % 12)][12]++;
                            }
                            st.prev_off.component = chord;
                            st.prev_off.set = true;
                        }
                    }
                    else {
                        // std::cout << "Nota normal\n";
                        if (st.prev_off.set) {
                            // std::cout << "Tem nota anterior: " << st.prev_off.key % 12 << ", nota atual: " << st.curr_ev.key % 12 << '\n';
                            tracks[i].note_matrix[(st.prev_off.component == chord ? 12 : st.prev_off.key % 12)][st.curr_ev.key % 12]++;                            
                        }
                        else {
                            // std::cout << "Não Tem nota anterior\n";
                        }
                        st.prev_off.ev = st.curr_ev.ev;
                        st.prev_off.key = st.curr_ev.key;
                        st.prev_off.component = note;
                        st.prev_off.set = true;
                    }
                    st.prev_note_type = off;
                    break;
                case time_signature:
                    // std::cout << "Mudança de compasso\n";// << st.curr_ev.ev.() << "\n";
                    break;
                case key_signature:
                    // std::cout << "Mudança de tom\n";// << st.curr_ev.ev.ge << "\n";
                    break;
                case tempo:
                    // tracks[i].tempos.push_back(TempoInfo());
                    // double bpm = st.curr_ev.ev.getTempoBPM();
                    [&tracks, i](double _bpm){
                        double bpm = _bpm;
                        if (fmod(bpm, 5)) {
                            int snap_dist = floor(bpm/5)*5;
                            bpm = (bpm - snap_dist < 2.5 ? snap_dist : snap_dist + 5);
                        }
                        tracks[i].bpm_timestamp[bpm].push_back(tracks[i].note_count);
                    }(st.curr_ev.ev->getTempoBPM());
                    // std::cout.precision(20);
                    // std::cout << "Mudança de tempo: " << st.curr_ev.ev.getTempoBPM() << "\n";
                    
                    break;
                case other:
                    // std::cout << "Other...\n";
                    break;
            }
            j++;
        }
        tracks[i].quarters.transitions = std::vector<std::vector<int>> {unique_quarters.size(), std::vector<int> {static_cast<int>(unique_quarters.size()), 0}};
        tracks[i].quarters.label = std::vector<int>(unique_quarters.begin(), unique_quarters.end());
        for (auto v : unique_quarters) {
            std::cout << v << " ";
        }
        std::cout << '\n';
        unique_quarters.clear();
        // display_note_ocurrence(tracks[i]);
        // std::cout << "Tempos: \n";
        // for (auto pair : tracks[i].bpm_timestamp) {
        //     std::cout << "Tempo: " << pair.first << " notestamps: ";
        //     for (auto notestamps : pair.second) {
        //         std::cout << notestamps << " ";
        //     }
        //     std::cout << "\n";
        // }
        // tracks[i].note_matrix = std::array<std::array<int, 13>, 13>{0};
    }
    
    // for (int i = 0; i < midifile.getTrackCount(); i++) {
        // for (int j = 1; j < ev_coll[i].note_on_evs.size(); j++) {
            // int q0 = std::nearbyint(ev_coll[i].note_on_evs[j-1]->getTickDuration() / midifile.getTicksPerQuarterNote());
            // int q = std::nearbyint(ev_coll[i].note_on_evs[j]->getTickDuration() / midifile.getTicksPerQuarterNote());
            // tracks[i].quarters.transitions[q0][q]++;
        // }
    // }
    // std::cout << "Tô aqui\n";

    for (int i = 0; i < midifile.getTrackCount(); i++) {
        for (auto &[duration, freq] : tracks[i].note_durations) {
            // std::cout.precision(17);
            // std::cout << "track " << i << ": " << duration << " - " << freq << '\n';
            // t += freq;
            feat.track_info.note_durations[duration] += freq;
        }
    }
    
    // int quarter_count = [&unique_quarters, tracks]()->int {
    //         int max = 0;
    //         for (int i = 0; i < tracks.size(); i++) {
    //             max = std::max((int)tracks[i].quarters.label.size(), max);
    //             for (auto label : tracks[i].quarters.label) {
    //                 unique_quarters.insert(label);
    //             }
    //         }
    //         return max;
    //     }();
    // std::map<int, int> quarter_label_index;
    // int i = 0;
    // for (auto label : unique_quarters) {
    //     quarter_label_index[label] = i++;
    // }

    // Soma matrizes de notas
    for (auto track : tracks) {
        for (int i = 0; i < 13; i++) {
            for (int j = 0; j < 13; j++) {
                feat.track_info.note_matrix[i][j] += track.note_matrix[i][j];    
            }
        }

        // for (int i = 0; i < track.quarters.transitions.size(); i++) {
        //     for (int j = 0; j < track.quarters.transitions.size(); j++) {
        //         feat.track_info.quarters.transitions[quarter_label_index[track.quarters.label[i]]][track.quarters.label[j]]++;   
        //     }
        // }
        
        for (auto a : feat.track_info.quarters.transitions) {
            for (auto b : a) {
                std::cout << b << " ";
            }
            std::cout << "\n";
        }
        
        for (auto bpm_freq_pair : track.bpm_timestamp) {
            feat.bpms.insert(bpm_freq_pair.first);
        }
        // int t{};
        for (auto &[duration, freq] : track.note_durations) {
            // std::cout.precision(17);
            // std::cout << duration << ": " << freq << '\n';
            // t += freq;
            feat.track_info.note_durations[duration] += freq;
        }
        // std::cout << t << '\n';
        
        feat.track_info.note_count += track.note_count;
    }
    // int t{};
    
    // for (auto &[duration, freq] : feat.track_info.note_durations) {
            // t += freq;
            // std::cout.precision(17);
            // std::cout << duration << ": " << freq << '\n';
            // feat.track_info.note_durations[duration] += freq;
        // }
        // std::cout << "Total: " <<  t << '\n';
    return feat;
}

void MidiAnalyzer::analyze_list(std::vector<MidiFSEntry> midi_list) {
    using nlohmann::json;
    std::vector<MidiFeatures> results;
    std::fstream f;

    if (midi_list.empty()) {
        DatabaseManager* dm = DatabaseManager::GetInstance();
        midi_list = dm->database;
    }

    for (auto midi_file_entry : midi_list) {
        results.push_back(analyze(midi_file_entry.path.string()));
    }
    json emotion_json;
    
    emotion_json["emotions"] = {
        {"happy", {}},
        {"relaxed", {}},
        {"angry", {}},
        {"sad", {}},
    };

    std::map<std::string, std::array<std::array<int, 13>, 13>> emotion_to_notes;
    std::map<std::string, int> emotion_to_note_count;
    std::map<std::string, std::set<int>> emotion_to_tempos;
    std::map<std::string, std::map<double, int>> emotion_to_durations;
    std::map<std::string, std::map<int, int>> emotion_to_quarters;
    json features_json;

    f.open("data/features2.json", std::ios::in);
    if (f.fail())
        std::cout << "deu ruim" << std::endl;
    else
        features_json = json::parse(f);         
    f.close();

    emotion_json["emotions"]["happy"]["songs"] = json::array();
    emotion_json["emotions"]["relaxed"]["songs"] = json::array();
    emotion_json["emotions"]["angry"]["songs"] = json::array();
    emotion_json["emotions"]["sad"]["songs"] = json::array();
    // emotion_json["emotions"]["sad"]["tempos"] = json::array();

    // std::cout << "TO AQUI\n";
    //     f.open("data/emotion_midi.json", std::ios::out);
    //     if (f.fail())
    //         std::cout << "deu ruim" << std::endl;
    //     else
    //         f << emotion_json.dump(4);
    //     f.close();
    // return;

    for (int i = 0; i < results.size(); i++) {
        // std::cout << midi_list[i] << '\n';
        // std::cout << spotify_ids[i] << '\n';
        // std::cout << "Note count: " << results[i].track_info.note_count << '\n';

        // std::cout << '\n';
        features_json["audio_features"][i]["midi_features"] = {
            {"note_matrix", results[i].track_info.note_matrix},
            {"note_count", results[i].track_info.note_count},
            {"bpms", results[i].bpms},
            {"durations", results[i].track_info.note_durations},
        };
        // auto it = std::find(features_json["audio_features"].begin(), features_json["audio_features"].end(), spotify_ids[i]);
        // if (it != features_json["audio_features"].end())
        //     std::cout << "Element found in myvector: " << *it << '\n';
        //     it
        // else
        //     std::cout << "Element not found in myvector\n";
        // std::cout << "\n\n";
        
        // emotion_json["emotions"][features_json["audio_features"][i]["emotion"]]["songs"].push_back(features_json["audio_features"][i]);
        for (int j = 0; j < 13; j++) {
            for (int k = 0; k < 13; k++) {
                emotion_to_notes[features_json["audio_features"][i]["emotion"]][j][k] += results[i].track_info.note_matrix[j][k];
            }
        }


        for (int tempos : features_json["audio_features"][i]["midi_features"]["bpms"]) {
            emotion_to_tempos[features_json["audio_features"][i]["emotion"]].insert(tempos);
        }

        for (auto &[duration, freq] : features_json["audio_features"][i]["midi_features"]["durations"].get<std::map<double, int>>()) { 
            emotion_to_durations[features_json["audio_features"][i]["emotion"]][duration] = freq;
        }   

        // emotion_to_quarters[features_json["audio_features"][i]["emotion"]];

        emotion_to_note_count[features_json["audio_features"][i]["emotion"]] += results[i].track_info.note_count;
        // std::cout << "\n\n";
    }

    for (auto &[emo, durations] : emotion_to_durations) {
        double sum{};
        std::map<double, double> duration_probs;
        std::for_each(durations.begin(), durations.end(), [&sum](std::pair<double, int> p){sum += p.second;});
        for (auto &[dur, freq] : durations) {        
            duration_probs[dur] = freq * 100.0 / sum;
        }         
        emotion_json["emotions"][emo]["durations_prob_matrix"] = duration_probs;
    }
    
    for (auto &[emo, matrix] : emotion_to_notes) {
        std::array<std::array<double, 13>, 13> prob_matrix;
        for (int i = 0; i < 13; i++) {
            double sum{};
            std::for_each(matrix[i].begin(), matrix[i].end(), [&sum](int a){sum += a;});
            for (int j = 0; j < 13; j++) {
                prob_matrix[i][j] = (double)matrix[i][j] * 100.0 / sum;
            }
        }
        // ticks_per_quarter = <PPQ from the header>
        // µs_per_quarter = <Tempo in latest Set Tempo event>
        // µs_per_tick = µs_per_quarter / ticks_per_quarter
        // seconds_per_tick = µs_per_tick / 1.000.000
        // seconds = ticks * seconds_per_tick
        emotion_json["emotions"][emo]["total_note_matrix"] = matrix;
        emotion_json["emotions"][emo]["prob_matrix"] =  prob_matrix;
        // emotion_json["emotions"][emo].push_back({"total_note_matrix", matrix});
        // emotion_json["emotions"][emo].push_back({"prob_matrix", prob_matrix});
        // emotion_json["emotions"][emo].push_back({
        //     {"total_note_matrix", matrix},
        //     {"prob_matrix", prob_matrix}
        // });
    }

    for (auto emo : EMO_TO_STR) {
        std::cout << "emocao: " << emo << '\n';
        emotion_json["emotions"][emo]["total_note_count"] = emotion_to_note_count[emo];
        emotion_json["emotions"][emo]["tempos"] = emotion_to_tempos[emo];
    }

    // f.open("data/features_midi.json", std::ios::out);
    // if (f.fail())
    //     std::cout << "deu ruim" << std::endl;
    // else
    //     f << features_json.dump(4);
    // f.close();

    f.open("data/emotion_midi.json", std::ios::out);
    if (f.fail())
        std::cout << "deu ruim" << std::endl;
    else
        f << emotion_json.dump(4);
    f.close();
}